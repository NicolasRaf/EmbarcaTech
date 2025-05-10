/* server.c - Correções para evitar PANIC no lwIP e usar conexão TCP persistente
 * Compilar com NO_SYS=1 (bare-metal) e ajustar lwipopts.h conforme abaixo:
 *   #define TCP_SND_QUEUELEN    32
 *   #define MEMP_NUM_TCP_SEG    32
 *   #define PBUF_POOL_SIZE      32
 */
#include "server.h"

static struct tcp_pcb *pcb = NULL;
static bool haveConnection = false;
static int retries = 0;
static volatile bool can_send = false;

// -----------------------------------------------------------------------------
// Callbacks
// -----------------------------------------------------------------------------

// Recebimento de dados
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (p == NULL) {
        // Conexão fechada pelo peer
        printf("Conexão fechada pelo peer\n");
        tcp_close(tpcb);
        pcb = NULL;
        haveConnection = false;
        return ERR_OK;
    }
    // Processa payload
    printf("Resposta recebida: %.*s\n", (int)p->tot_len, (char*)p->payload);
    // Sinaliza recepção e libera pbuf
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    return ERR_OK; // manter conexão (keep-alive)
}

// Confirmação de envio (ACK)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    LWIP_UNUSED_ARG(arg);
    can_send = true;
    return ERR_OK;
}

// Erro na conexão
void tcp_client_error(void *arg, err_t err) {
    LWIP_UNUSED_ARG(arg);
    printf("Erro TCP (%d). Resetando conexão...\n", err);
    pcb = NULL;
    haveConnection = false;
    if (retries < MAX_RETRIES) create_tcp_connection();
}

// Conexão estabelecida
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (err != ERR_OK) return err;
    printf("Conectado ao proxy!\n");
    pcb = tpcb;
    haveConnection = true;
    can_send = true;
    // Registra callbacks
    tcp_arg(pcb, NULL);
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);
    return ERR_OK;
}

// -----------------------------------------------------------------------------
// DNS callback
// -----------------------------------------------------------------------------

void dns_proxy_callback(const char *name, const ip_addr_t *ip, void *arg) {
    LWIP_UNUSED_ARG(name);
    LWIP_UNUSED_ARG(arg);
    if (ip == NULL) {
        printf("Falha ao resolver DNS: %s\n", name);
        return;
    }
    pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB apos DNS\n");
        return;
    }
    err_t ret = tcp_connect(pcb, ip, PROXY_PORT, tcp_client_connected);
    if (ret != ERR_OK) {
        printf("Erro tcp_connect apos DNS (%d)\n", ret);
        tcp_abort(pcb);
        pcb = NULL;
    }
}

// -----------------------------------------------------------------------------
// Envio TCP com controle de fluxo
// -----------------------------------------------------------------------------

static void do_tcp_write(const char *data, size_t len) {
    if (!pcb || !haveConnection) {
        printf("Cannot send: no active connection\n");
        return;
    }
    size_t sent = 0;
    while (sent < len) {
        uint16_t mss = tcp_mss(pcb);
        size_t chunk = LWIP_MIN(mss, len - sent);
        // Aguarda liberação de ACK, espaço no sendbuf e queue disponível
        while (!can_send || tcp_sndbuf(pcb) < chunk || tcp_sndqueuelen(pcb) + 1 >= TCP_SND_QUEUELEN) {
            sys_check_timeouts();
            cyw43_arch_poll();
        }
        can_send = false;
        err_t werr = tcp_write(pcb, data + sent, chunk, TCP_WRITE_FLAG_COPY);
        if (werr != ERR_OK) {
            printf("Erro tcp_write: %d\n", werr);
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }
        tcp_output(pcb);
        sent += chunk;
    }
}

// -----------------------------------------------------------------------------
// Gestão de conexão e envio periódico
// -----------------------------------------------------------------------------

void create_tcp_connection(void) {
    if (pcb) return;
    retries++;
    ip_addr_t ip;
    err_t err = dns_gethostbyname(PROXY_HOST, &ip, dns_proxy_callback, NULL);
    if (err == ERR_OK) {
        dns_proxy_callback(PROXY_HOST, &ip, NULL);
    } else if (err != ERR_INPROGRESS) {
        printf("dns_gethostbyname falhou (%d)\n", err);
    }
}

void send_data_to_server(void) {
    static uint32_t last = 0;
    uint32_t now = sys_now();
    if (now - last < SEND_INTERVAL_MS) return;
    last = now;
    sys_check_timeouts();
    cyw43_arch_poll();
    if (!pcb || !haveConnection) {
        if (retries < MAX_RETRIES) create_tcp_connection();
        return;
    }
    // Monta JSON
    char json[256];
    int jlen = snprintf(json, sizeof(json),
        "{\"eixo_x\":%d,\"eixo_y\":%d,\"direcao\":\"%s\",\"botao_1\":\"%s\",\"botao_2\":\"%s\",\"hc-sr04\":%.2f}",
        axisX, axisY, joystickDirection,
        buttonAState, buttonBState,
        lastDistance);
    // Monta requisição HTTP com keep-alive
    char req[512];
    int rlen = snprintf(req, sizeof(req),
        "POST /update HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n\r\n"
        "%s",
        PROXY_HOST, jlen, json);
    do_tcp_write(req, rlen);
    retries = 0;
}