#include "server.h"

struct tcp_pcb *pcb = NULL;
bool haveConnection = false;
int  retries = 0;
static volatile bool can_send = false;

// Callback ao receber resposta do servidor
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        // Servidor fechou conexão
        printf("Conexão fechada pelo servidor.\n");
        tcp_close(tpcb);
        pcb = NULL;
        haveConnection = false;
        return ERR_OK;
    }
    // Imprime resposta
    printf("Resposta recebida: %.*s\n", (int)p->tot_len, (char *)p->payload);
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
    // Fechamos após receber resposta (Connection: close)
    tcp_close(tpcb);
    pcb = NULL;
    haveConnection = false;
    return ERR_OK;
}

// Callback quando dados são efetivamente ACKed
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    LWIP_UNUSED_ARG(arg);
    can_send = true;
    return ERR_OK;
}

// Callback para erro TCP
void tcp_client_error(void *arg, err_t err) {
    LWIP_UNUSED_ARG(arg);
    printf("Erro na conexão TCP (%d). Tentando reconectar...\n", err);
    pcb = NULL;
    haveConnection = false;
    if (retries < MAX_RETRIES) {
        retries++;
        create_tcp_connection();
    } else {
        printf("Máximo de tentativas atingido. Abortando.\n");
    }
}

// Callback ao conectar com sucesso
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {
    LWIP_UNUSED_ARG(arg);
    if (err != ERR_OK) {
        printf("Erro ao conectar (%d)\n", err);
        return err;
    }
    printf("Conectado ao servidor com sucesso!\n");
    pcb = tpcb;
    haveConnection = true;
    can_send      = true;                // libera o primeiro envio
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);
    return ERR_OK;
}

// Criar conexão TCP
void create_tcp_connection(void) {
    if (pcb) return;

    ip_addr_t proxy_ip;
    ip4addr_aton("192.168.137.7", &proxy_ip);

    pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }
    tcp_arg(pcb, NULL);
    tcp_err(pcb, tcp_client_error);
    tcp_recv(pcb, tcp_client_recv);
    tcp_sent(pcb, tcp_client_sent);

    err_t ret = tcp_connect(pcb, &proxy_ip, 3000, tcp_client_connected);
    if (ret != ERR_OK) {
        printf("Falha na conexão (%d). Tentativa %d/%d\n", ret, retries+1, MAX_RETRIES);
        tcp_abort(pcb);
        pcb = NULL;
        retries++;
    }
}

// Envia dados fragmentados e com controle de fila
static void do_tcp_write(const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        uint16_t mss   = tcp_mss(pcb);
        size_t   chunk = LWIP_MIN(mss, len - sent);

        // espera espaço livre e ACK anterior
        while (!can_send ||
               pcb->snd_queuelen + ((chunk + mss - 1) / mss) > TCP_SND_QUEUELEN) {
            sys_msleep(1);
        }
        can_send = false;

        if (tcp_write(pcb, data + sent, chunk, TCP_WRITE_FLAG_COPY) != ERR_OK ||
            tcp_output(pcb)  != ERR_OK) {
            printf("Erro ao enviar dados. Abortando conexão.\n");
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }
        sent += chunk;
    }
}

// Chame esta função periodicamente (ex: no loop principal)
void send_data_to_server(void) {
    static uint32_t last = 0;
    uint32_t now = sys_now();
    if (now - last < SEND_INTERVAL_MS) return;
    last = now;

    // Processa timeouts do lwIP
    sys_check_timeouts();

    // tenta (re)conectar se preciso
    if (!haveConnection || !pcb) {
        if (retries < MAX_RETRIES) {
            printf("Reconectando (%d/%d)...\n", retries+1, MAX_RETRIES);
            create_tcp_connection();
        }
        return;
    }

    // Monta JSON
    char json[2048];
    int  jlen = snprintf(json, sizeof(json),
        "{\"eixo_x\":%d,\"eixo_y\":%d,"  
        "\"direcao\":\"%s\",\"botao_1\":\"%s\",\"botao_2\":\"%s\","  
        "\"hc-sr04\":%.2f}",
        x_value, y_value, directionWindRose,
        button1_state, button2_state, lastDistance
    );

    // Monta requisição HTTP com Connection: close
    char req[1024];
    int  rlen = snprintf(req, sizeof(req),
        "POST /update HTTP/1.1\r\n"
        "Host: 192.168.137.7\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        jlen, json
    );

    // Envia e fecha automaticamente após receive
    do_tcp_write(req, rlen);
}

// Fecha conexão TCP
void close_tcp_connection(void) {
    if (pcb) {
        tcp_close(pcb);
        pcb = NULL;
        haveConnection = false;
        printf("Conexão TCP fechada.\n");
    }
}

// Reseta e reconecta
void resetConnection(void) {
    printf("Resetando conexão...\n");
    blinkWarn();
    close_tcp_connection();
    retries = 0;
    create_tcp_connection();
    turnOffLeds();
}
