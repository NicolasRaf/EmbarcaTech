#include "server.h"

static struct tcp_pcb *pcb = NULL;
static bool haveConnection = false;
static int retries = 0;
static volatile bool can_send = false;

// Recebimento de dados
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        tcp_close(tpcb);
        pcb = NULL;
        haveConnection = false;
        return ERR_OK;
    }
    printf("Resposta recebida: %.*s\n", (int)p->tot_len, (char*)p->payload);
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    tcp_close(tpcb);
    pcb = NULL;
    haveConnection = false;
    return ERR_OK;
}

// Confirmação de dados enviados (ACK)
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    LWIP_UNUSED_ARG(arg);
    can_send = true;
    return ERR_OK;
}

// Erro na conexão
void tcp_client_error(void *arg, err_t err) {
    LWIP_UNUSED_ARG(arg);
    printf("Erro TCP (%d). Reconectando...\n", err);
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
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);
    return ERR_OK;
}

// ------------------ TCP Write with queue check ------------------
static void do_tcp_write(const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        uint16_t mss   = tcp_mss(pcb);
        size_t   chunk = LWIP_MIN(mss, len - sent);
        // espera ACK e espaço em bytes
        while (!can_send || tcp_sndbuf(pcb) < chunk) {
            sys_msleep(1);
        }
        can_send = false;
        if (tcp_write(pcb, data + sent, chunk, TCP_WRITE_FLAG_COPY) != ERR_OK ||
            tcp_output(pcb) != ERR_OK) {
            // aborta no erro
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }
        sent += chunk;
    }
}


// ------------------ Connection Management ------------------

void dns_proxy_callback(const char *name, const ip_addr_t *ip, void *arg) {
    if (ip == NULL) {
        printf("Falha ao resolver DNS: %s\n", name);
        return;
    }


    pcb = tcp_new();
    tcp_arg(pcb, NULL);
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);
    tcp_connect(pcb, ip, PROXY_PORT, tcp_client_connected);
}

void create_tcp_connection(void) {
    ip_addr_t ip;
    err_t err = dns_gethostbyname(PROXY_HOST, &ip,
                                  dns_proxy_callback, NULL);
    if (err == ERR_OK) {
        // cache: conectar imediatamente
        dns_proxy_callback(PROXY_HOST, &ip, NULL);
    } else if (err != ERR_INPROGRESS) {
        printf("dns_gethostbyname falhou (%d)\n", err);
    }
}


void close_tcp_connection(void) {
    if (pcb) {
        tcp_close(pcb);
        pcb = NULL;
        haveConnection = false;
        printf("Conexão TCP fechada.\n");
    }
}

// ------------------ Periodic Send ------------------

void send_data_to_server(void) {
    static uint32_t last = 0;
    uint32_t now = sys_now();
    if (now - last < SEND_INTERVAL_MS) return;
    last = now;
    sys_check_timeouts();

    if (!haveConnection || !pcb) {
        if (retries < MAX_RETRIES) {
            printf("Reconectando... (%d/%d)\n", retries, MAX_RETRIES);
            create_tcp_connection();
        }
        return;
    }
    // Monta JSON
    char json[512];
    int jlen = snprintf(json, sizeof(json),
        "{\"eixo_x\":%d,\"eixo_y\":%d,\"direcao\":\"%s\","
        "\"botao_1\":\"%s\",\"botao_2\":\"%s\",\"hc-sr04\":%.2f}",
        axisX, axisY, joystickDirection, buttonAState, buttonBState, lastDistance);
    // Monta POST
    char req[600];
    int rlen = snprintf(req, sizeof(req),
        "POST /update HTTP/1.1\r\n"
        "Host: vigia_das_aguas-server.railway.internal\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n\r\n%s",
        jlen, json);
    do_tcp_write(req, rlen);
    retries = 0;
}
