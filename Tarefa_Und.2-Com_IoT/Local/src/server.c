#include "server.h"

// Callback chamado quando os dados foram enviados (fecha a conexão)
static err_t on_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    tcp_close(tpcb);
    return ERR_OK;
}

// Callback principal de recepção
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Copia a requisição para string terminada em '\0'
    char *request = malloc(p->tot_len + 1);
    if (!request) {
        pbuf_free(p);
        return ERR_MEM;
    }
    memcpy(request, p->payload, p->tot_len);
    request[p->tot_len] = '\0';

    {
        // extrai a primeira linha até '\r' para não poluir o log
        char line[128] = {0};
        sscanf(request, "%127[^\r\n]", line);
        printf("[HTTP REQ] %s\n", line);
    }

    // Marca bytes recebidos e libera o pbuf
    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);

    // Atualiza leitura de sensores e botões
    read_joystick();
    update_button_states();
    measureDistance();

    bool is_data = (strstr(request, "GET /data") != NULL);
    static char response_buffer[1024];
    err_t wr_err;

    if (is_data) {
        // Monta corpo JSON
        char json_body[256];
        int json_len = snprintf(json_body, sizeof(json_body),
            "{\"x\":%d,\"y\":%d,\"direction\":\"%s\",\"button1\":%d,\"button2\":%d,\"ultra\":%f}",
            x_value, y_value, directionWindRose, button1_state, button2_state, lastDistance);

        // Monta header
        int header_len = snprintf(response_buffer, sizeof(response_buffer),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n",
            json_len);

        // Concatena e envia
        memcpy(response_buffer + header_len, json_body, json_len);
        tcp_sent(tpcb, on_sent);
        wr_err = tcp_write(tpcb, response_buffer, header_len + json_len, TCP_WRITE_FLAG_COPY);
        if (wr_err != ERR_OK) printf("Erro ao enviar JSON: %d\n", wr_err);
    } else {
        // Envia o HTML estático de html_response.h
        int total_len = strlen(html_response);
        tcp_sent(tpcb, on_sent);
        wr_err = tcp_write(tpcb, html_response, total_len, TCP_WRITE_FLAG_COPY);
        if (wr_err != ERR_OK) printf("Erro ao enviar HTML: %d\n", wr_err);
    }

    tcp_output(tpcb);
    free(request);
    return ERR_OK;
}

// Aceita nova conexão e registra callback de recepção
static err_t on_http_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_recv_callback);
    return ERR_OK;
}

// Inicializa e inicia o servidor na porta definida
void start_server() {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Error creating PCB\n");
        return;
    }

    err_t bind_err = tcp_bind(pcb, IP_ADDR_ANY, PORT);
    if (bind_err != ERR_OK) {
        printf("Bind error: %d\n", bind_err);
        return;
    }

    pcb = tcp_listen_with_backlog(pcb, 1);
    if (!pcb) {
        printf("Error: PCB is null\n");
        return;
    }

    tcp_accept(pcb, on_http_accept);
    printf("Server started on port %d\n", PORT);
}
