#include "server.h"

static struct tcp_pcb *pcb = NULL;
static bool haveConnection = false;
static volatile bool can_send = false;
static int retries = 0;

// -----------------------------------------------------------------------------
// Callbacks
// -----------------------------------------------------------------------------

// Callback para recebimento de dados
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (p == NULL) {
        // Peer fechou conexão
        tcp_close(tpcb);
        pcb = NULL;
        haveConnection = false;
        return ERR_OK;
    }

    // Notifica lwIP que consumimos os bytes
    tcp_recved(tpcb, p->tot_len);

    // Libera cada pbuf da cadeia para evitar double-free
    struct pbuf *q = p;
    while (q) {
        struct pbuf *next = q->next;
        q->next = NULL;
        pbuf_free(q);
        q = next;
    }
    return ERR_OK;
}

// Callback para confirmação de envio (ACK)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    LWIP_UNUSED_ARG(arg);
    can_send = true;
    return ERR_OK;
}

// Callback de erro de conexão
void tcp_client_error(void *arg, err_t err) {
    LWIP_UNUSED_ARG(arg);
    printf("Erro TCP: %d\n", err);
    pcb = NULL;
    haveConnection = false;
}

// Callback de conexão estabelecida
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (err != ERR_OK) {
        printf("Falha ao conectar: %d\n", err);
        return err;
    }
    printf("Conectado ao proxy!\n");
    pcb = tpcb;
    haveConnection = true;
    can_send = true;
    tcp_arg(pcb, NULL);
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);
    return ERR_OK;
}

// -----------------------------------------------------------------------------
// Criação de conexão via DNS
// -----------------------------------------------------------------------------

void dns_proxy_callback(const char *name, const ip_addr_t *ip, void *arg) {
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(arg);
    if (ip == NULL) {
        printf("Falha DNS: %s\n", name);
        return;
    }
    pcb = tcp_new();
    if (pcb == NULL) {
        printf("Erro tcp_new()\n");
        return;
    }
    err_t ret = tcp_connect(pcb, ip, PROXY_PORT, tcp_client_connected);
    if (ret != ERR_OK) {
        printf("Erro tcp_connect: %d\n", ret);
        tcp_abort(pcb);
        pcb = NULL;
    }
}

void create_tcp_connection(void) {
    if (pcb) return;
    if (++retries > MAX_RETRIES) {
        printf("Max retries reached\n");
        return;
    }
    ip_addr_t ip;
    err_t err = dns_gethostbyname(PROXY_HOST, &ip, dns_proxy_callback, NULL);
    if (err == ERR_OK) {
        dns_proxy_callback(PROXY_HOST, &ip, NULL);
    } else if (err != ERR_INPROGRESS) {
        printf("dns_gethostbyname falhou: %d\n", err);
    }
}

// -----------------------------------------------------------------------------
// Envio TCP com controle de fluxo e prevenção de panic
// -----------------------------------------------------------------------------

/**
 * Envia dados via TCP com controle de fluxo e prevenção de panic.
 *
 * Verifica se a conexão está estabelecida e se pode enviar dados.
 * Se sim, envia os dados em chunks menores que o MSS (Maximum Segment Size)
 * e aguarda um tempo razoável pelo ACK (acknowledgment) do servidor.
 * Se o servidor demorar muito para responder, aborta a conexão.
 *
 * @param data Ponteiro para a string a ser enviada
 * @param len Tamanho da string a ser enviada
 */
static void do_tcp_write(const char *data, size_t len) {
    if (!pcb || !haveConnection || !data || len == 0) {
        printf("Invalid parameters or connection state\n");
        return;
    }

    size_t sent = 0;
    while (sent < len) {
        uint16_t mss = tcp_mss(pcb);         // Maximum segment size
        size_t remaining = len - sent;       // Remaining data to send
        size_t chunk = LWIP_MIN(mss, remaining);  // Size of the next data chunk

        absolute_time_t start = get_absolute_time();

        // Check if we can send
        while (!can_send || (tcp_sndqueuelen(pcb) + 1 >= TCP_SND_QUEUELEN) || (tcp_sndbuf(pcb) < chunk)) {
            printf("Waiting to send (ack=%d, buf=%d, queue=%d/%d)\n",
                   can_send, tcp_sndbuf(pcb), tcp_sndqueuelen(pcb), TCP_SND_QUEUELEN);

            sys_check_timeouts();
            cyw43_arch_poll();

            // Timeout after 2 seconds
            if (absolute_time_diff_us(start, get_absolute_time()) > 2000000) {
                printf("Flow timeout, aborting\n");
                tcp_abort(pcb);
                pcb = NULL;
                haveConnection = false;
                return;
            }
        }

        can_send = false;  // Prevent sending before ACK

        // Send data
        if (tcp_write(pcb, data + sent, chunk, TCP_WRITE_FLAG_COPY) != ERR_OK) {
            printf("tcp_write() error\n");
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }

        // Ensure TCP sends the data correctly
        if (pcb != NULL && tcp_output(pcb) != ERR_OK) {
            printf("Error sending data with tcp_output\n");
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }

        sent += chunk;  // Advance the amount of data sent
    }
}

// -----------------------------------------------------------------------------
// Envio periódico e gerenciamento de conexões
// -----------------------------------------------------------------------------

void send_data_to_server(void) {
    static uint32_t last = 0;
    uint32_t now = sys_now();
    if (now - last < SEND_INTERVAL_MS) return;
    last = now;
    sys_check_timeouts();
    cyw43_arch_poll();
    if (!pcb || !haveConnection) {
        create_tcp_connection();
        return;
    }
    // Monta JSON
    char json[256];
    int jlen = snprintf(json, sizeof(json),
        "{\"eixo_x\":%d,\"eixo_y\":%d,\"direcao\":\"%s\",\"botao_1\":\"%s\",\"botao_2\":\"%s\",\"hc-sr04\":%.2f}",
        axisX, axisY, joystickDirection,
        buttonAState, buttonBState,
        lastDistance);
    if (jlen < 0 || jlen >= sizeof(json)) {
        printf("Erro montar JSON\n");
        return;
    }
    // Monta requisição POST
    char req[512];
    int rlen = snprintf(req, sizeof(req),
        "POST /update HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n\r\n"
        "%s",
        PROXY_HOST, jlen, json);
    if (rlen < 0 || rlen >= sizeof(req)) {
        printf("Erro montar requisicao\n");
        return;
    }
    // Log timestamp de envio
    uint32_t ms_now = sys_now();
    uint32_t s = ms_now / 1000;
    printf("Enviando %d bytes em %02u:%02u:%02u\n", rlen,
           (s/3600)%24, (s/60)%60, s%60);
    // Envia os dados
    do_tcp_write(req, rlen);
    retries = 0;
}
