#include "server.h"

struct tcp_pcb *pcb = NULL;
bool haveConnection = false;
int retries = 0;
#define MAX_RETRIES 5

// Callback ao receber resposta do servidor
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (p == NULL) {
        printf("Conexão fechada pelo servidor.\n");
        tcp_close(tpcb);
        haveConnection = false;
        pcb = NULL;
        return ERR_OK;
    } else {
        printf("Resposta recebida: %s\n", (char *)p->payload);
        tcp_recved(tpcb, p->tot_len);
        pbuf_free(p);
        return ERR_OK;
    }
}

// Callback para erro TCP
void tcp_client_error(void *arg, err_t err) {
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
    if (err != ERR_OK) {
        printf("Erro ao conectar (%d)\n", err);
        return err;
    }

    printf("Conectado ao servidor com sucesso!\n");
    haveConnection = true;
    tcp_recv(tpcb, tcp_client_recv);
    return ERR_OK;
}

// Criar conexão TCP
void create_tcp_connection() {
    if (pcb != NULL) return; // Já está conectado

    const char *proxy_ip_str = "192.168.43.160";  // IP do seu servidor
    int proxy_port = 3000;

    ip_addr_t proxy_ip;
    ip4addr_aton(proxy_ip_str, &proxy_ip);

    pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    tcp_err(pcb, tcp_client_error);  // Callback de erro

    err_t connect_err = tcp_connect(pcb, &proxy_ip, proxy_port, tcp_client_connected);
    if (connect_err != ERR_OK) {
        printf("Falha na conexão (%d). Tentativa %d/%d\n", connect_err, retries + 1, MAX_RETRIES);
        tcp_abort(pcb);
        pcb = NULL;
        retries++;
    } 
}

// Enviar dados ao servidor
void send_data_to_server() {
    while (pcb == NULL || !haveConnection) {
        if (retries >= MAX_RETRIES) {
            printf("Erro: Não foi possível restabelecer a conexão após %d tentativas.\n", MAX_RETRIES);
            return;
        }

        printf("Conexão perdida. Tentando reconectar... (%d/%d)\n", retries + 1, MAX_RETRIES);
        create_tcp_connection();
        sleep_ms(1000);
    }

    retries = 0; // Resetar contagem

    printf("\nCriando JSON");
    // Criar JSON
    char json[2048];
    snprintf(json, sizeof(json),
        "{"
        "\"eixo_x\": %d, "
        "\"eixo_y\": %d, "
        "\"direcao\": \"%s\", "
        "\"botao_1\": \"%s\", "
        "\"botao_2\": \"%s\", "
        "\"hc-sr04\": %.2f"
        "}",
        x_value, y_value, directionWindRose, button1_state, button2_state, lastDistance
    );

    const char *host = "192.168.43.160";

    printf("\nCriando requisição HTTP");
    // Criar requisição HTTP
    char request[1024];
    snprintf(request, sizeof(request),
        "POST /update HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: keep-alive\r\n"
        "\r\n"
        "%s",
        host,
        (int)strlen(json),
        json
    );

    printf("\nEnviando JSON:\n%s\n", json);

    // Enviar dados
    if (tcp_write(pcb, request, strlen(request), TCP_WRITE_FLAG_COPY) != ERR_OK) {
        printf("Erro ao escrever no socket. Fechando conexão...\n");
        tcp_abort(pcb);
        pcb = NULL;
        haveConnection = false;
        return;
    }

    if (tcp_output(pcb) != ERR_OK) {
        printf("Erro ao enviar dados (tcp_output). Fechando conexão...\n");
        tcp_abort(pcb);
        pcb = NULL;
        haveConnection = false;
        return;
    }
}

// Encerrar conexão TCP
void close_tcp_connection() {
    if (pcb != NULL) {
        tcp_close(pcb);
        pcb = NULL;
        haveConnection = false;
        printf("Conexão TCP fechada.\n");
    }
}

void resetConnection() {
    printf("Resetando conexão...\n");
    blinkWarn();

    close_tcp_connection();
    retries = 0;
    haveConnection = false;
    create_tcp_connection();
}