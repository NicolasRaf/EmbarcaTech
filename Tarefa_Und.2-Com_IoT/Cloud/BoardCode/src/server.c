#include "server.h"

// Variáveis globais
struct tcp_pcb *pcb = NULL;            // Conexão TCP
bool haveConnection = false;           // Conexão aberta ou fechada
int  retries = 0;                      // Tentativas de reconexão
static volatile bool can_send = false; // Flag para permitir envio de dados

/**
 * Callback para recebimento de dados TCP.
 *
 * Imprime a resposta recebida e fecha a conexão.
 *
 * @param arg Ponteiro para o objeto de conexão TCP.
 * @param tpcb Ponteiro para o PCB da conexão TCP.
 * @param p Ponteiro para o buffer de dados recebidos.
 * @param err Código de erro (se houver).
 *
 * @return ERR_OK em caso de sucesso, ou outro código de erro.
 */
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

/**
 * Callback para confirmação de envio de dados TCP.
 *
 * Libera o flag de envio de dados.
 *
 * @param arg Ponteiro para o objeto de conexão TCP (não usado).
 * @param tpcb Ponteiro para o PCB da conexão TCP.
 * @param len Tamanho dos dados enviados.
 *
 * @return ERR_OK em caso de sucesso, ou outro código de erro.
 */
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len) {
    LWIP_UNUSED_ARG(arg);
    can_send = true;
    return ERR_OK;
}

/**
 * Callback para erros TCP.
 *
 * Imprime mensagem de erro e fecha a conexão. Caso o número de tentativas
 * seja inferior ao máximo, tenta reconectar. Caso contrário, aborta.
 *
 * @param arg Ponteiro para o objeto de conexão TCP (não usado).
 * @param err Código de erro (se houver).
 */
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

/**
 * Callback ao conectar com sucesso.
 *
 * @param arg Ponteiro para o objeto de conexão TCP (não usado).
 * @param tpcb Ponteiro para o PCB da conexão TCP.
 * @param err Código de erro (se houver).
 *
 * @return ERR_OK em caso de sucesso, ou outro código de erro.
 */
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

/**
 * Cria uma conexão TCP com o servidor.
 *
 * Se a conexão estiver aberta, simplesmente retorna.
 *
 * @note A conexão é criada com o endereço IP do servidor e a porta 1111.
 */
void create_tcp_connection(void) {
    if (pcb) return;

    // Endereço IP do servidor
    ip_addr_t proxy_ip;
    ip4addr_aton("192.168.0.5", &proxy_ip);

    // Cria um novo PCB
    pcb = tcp_new();
    if (!pcb) {
        printf("Erro ao criar PCB\n");
        return;
    }

    // Configura o PCB
    tcp_arg(pcb, NULL);                  // Ponteiro genérico (não usado)
    tcp_err(pcb, tcp_client_error);      // Função de erro
    tcp_recv(pcb, tcp_client_recv);      // Função de recebimento de dados
    tcp_sent(pcb, tcp_client_sent);      // Função de confirmação de envio

    // Tenta conectar ao servidor
    err_t ret = tcp_connect(pcb, &proxy_ip, 3000, tcp_client_connected);
    if (ret != ERR_OK) {
        printf("Falha na conexão (%d). Tentativa %d/%d\n", ret, retries+1, MAX_RETRIES);
        tcp_abort(pcb);                  // Aborta a conexão
        pcb = NULL;
        retries++;                       // Incrementa o contador de tentativas
    }
}

/**
 * Envia dados fragmentados e com controle de fila.
 *
 * Fragmenta os dados em chunks que cabem na janela de envio do TCP
 * (limitada pelo MSS) e envia cada chunk com tcp_write().
 * Verifica se há espaço livre na fila de envio e se o ACK anterior
 * foi recebido antes de enviar o próximo chunk.
 *
 * @param data Ponteiro para os dados a serem enviados.
 * @param len  Tamanho dos dados a serem enviados.
 */
static void do_tcp_write(const char *data, size_t len) {
    size_t sent = 0;
    while (sent < len) {
        uint16_t mss   = tcp_mss(pcb);
        size_t   chunk = LWIP_MIN(mss, len - sent);

        // Aguarda espaço livre e ACK anterior
        while (!can_send ||
               pcb->snd_queuelen + ((chunk + mss - 1) / mss) > TCP_SND_QUEUELEN) {
            sys_msleep(1);
        }

        // Libera o flag de envio
        can_send = false;

        // Envia chunk
        if (tcp_write(pcb, data + sent, chunk, TCP_WRITE_FLAG_COPY) != ERR_OK ||
            tcp_output(pcb)  != ERR_OK) {
            printf("Erro ao enviar dados. Abortando conexão.\n");
            tcp_abort(pcb);
            pcb = NULL;
            haveConnection = false;
            return;
        }

        // Incrementa contador de bytes enviados
        sent += chunk;
    }
}

/**
 * Envia dados para o servidor periodicamente.
 *
 * Chame esta função periodicamente (ex: no loop principal)
 * para enviar os dados atuais para o servidor.
 */
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
        axisY, axisX, joystickDirection,
        buttonAState, buttonBState, lastDistance
    );

    // Monta requisi o HTTP com Connection: close
    char req[1024];
    int  rlen = snprintf(req, sizeof(req),
        "POST /update HTTP/1.1\r\n"
        "Host: 192.168.0.5\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "Connection: close\r\n"
        "\r\n"
        "%s",
        jlen, json
    );

    // Envia e fecha automaticamente ap s receive
    do_tcp_write(req, rlen);
}

/**
 * Fecha a conexão TCP.
 *
 * Chama tcp_close() para fechar a conexão TCP e
 * reseta as variáveis globais.
 */
void close_tcp_connection(void) {
    if (pcb) {
        tcp_close(pcb);
        pcb = NULL;
        haveConnection = false;
        printf("Conexão TCP fechada.\n");
    }
}

/**
 * Reseta e reconecta.
 *
 * Fecha a conexão TCP, reseta as variáveis globais e
 * tenta reconectar ao servidor.
 */
void resetConnection(void) {   
    if (haveConnection || pcb) return;

    printf("Resetando conexão...\n");
    blinkWarn();
    
    // Fecha a conexão
    close_tcp_connection();
    retries = 0;
    create_tcp_connection();

    turnOffLeds();
}
