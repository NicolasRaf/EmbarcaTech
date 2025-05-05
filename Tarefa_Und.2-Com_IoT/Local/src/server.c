#include "server.h"

char directionWindRose[3] = "C";
int x_value = 0;
int y_value = 0;

void calculate_direction() {
    if (x_value > 3000 && y_value > 3000) strcpy(directionWindRose, "NE");
    else if (x_value > 3000 && y_value < 1000) strcpy(directionWindRose, "SE");
    else if (x_value < 1000 && y_value > 3000) strcpy(directionWindRose, "NW");
    else if (x_value < 1000 && y_value < 1000) strcpy(directionWindRose, "SW");
    else if (x_value > 3000) strcpy(directionWindRose, "E");
    else if (x_value < 1000) strcpy(directionWindRose, "W");
    else if (y_value > 3000) strcpy(directionWindRose, "N");
    else if (y_value < 1000) strcpy(directionWindRose, "S");
    else strcpy(directionWindRose, "C");
}

void read_joystick() {
    adc_select_input(0); // ADC0 - GPIO26
    x_value = adc_read();
    adc_select_input(1); // ADC1 - GPIO27
    y_value = adc_read();
    calculate_direction();
}

err_t on_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    tcp_close(tpcb);
    return ERR_OK;
}

err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        tcp_close(tpcb);
        return ERR_OK;
    }

    char *req = p->payload;
    printf("%s", req);

    read_joystick();
    update_button_states();

    err_t wr_err;

    if (strstr(req, "GET /data") != NULL && strstr(req, "HTTP/") != NULL) {
        // Envia os dados JSON
        char json[256];
        snprintf(json, sizeof(json),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: application/json\r\n"
            "Connection: close\r\n"
            "\r\n"
            "{\"x\": %d, \"y\": %d, \"direction\": \"%s\", \"button1\": %d, \"button2\": %d}",
            x_value, y_value, directionWindRose, button1_state, button2_state);

        tcp_sent(tpcb, on_sent);
        wr_err = tcp_write(tpcb, json, strlen(json), TCP_WRITE_FLAG_COPY);
        if (wr_err != ERR_OK) printf("Erro ao enviar JSON: %d\n", wr_err);
        tcp_output(tpcb);
    } else {
        // Para qualquer outra rota, envia o HTML
        char response[512];
        snprintf(response, sizeof(response),
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: %d\r\n"
            "\r\n",
            strlen(html_response));

        // Envia o cabeçalho
        tcp_sent(tpcb, on_sent);
        wr_err = tcp_write(tpcb, response, strlen(response), TCP_WRITE_FLAG_COPY);
        if (wr_err != ERR_OK) {
            printf("Erro ao enviar cabeçalho: %d\n", wr_err);
        }
        // Envia o conteúdo HTML
        wr_err = tcp_write(tpcb, html_response, strlen(html_response), TCP_WRITE_FLAG_COPY);
        if (wr_err != ERR_OK) {
            printf("Erro ao enviar HTML: %d\n", wr_err);
        }
        tcp_output(tpcb);
    }

    pbuf_free(p);
    return ERR_OK;
}



err_t on_http_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, tcp_recv_callback);
    return ERR_OK;
}

void start_server() {
    struct tcp_pcb *pcb = tcp_new();
    if (!pcb) {
        printf("Error creating PCB\n");
        return;
    }

    err_t err = tcp_bind(pcb, IP_ADDR_ANY, PORT);
    if (err != ERR_OK) {
        printf("Bind error: %d\n", err);
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

