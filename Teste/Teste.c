#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "music.h"

// Configurações de Wi-Fi (alterar para o SSID e senha da sua rede)
#define WIFI_SSID "Amora"
#define WIFI_PASSWORD "Naruto2021"

// Pino do LED vermelho do BitDogLab (ou LED configurado no seu hardware)
#define RED_LED_PIN 13
#define GREEN_LED_PIN 11

// Página HTML enviada pelo servidor para o controle do LED e música
const char *html_page = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/html\r\n\r\n"
                        "<!DOCTYPE html><html><body>"
                        "<h1>Pico W LED & Music Control</h1>"
                        "<p><a href=\"/?led=on\"><button>Ligar LED</button></a></p>"
                        "<p><a href=\"/?led=off\"><button>Desligar LED</button></a></p>"
                        "<p><a href=\"/?music=play\"><button>Play Music</button></a></p>"
                        "</body></html>";

/**
 * Manipulador de requisições HTTP.
 * Recebe os dados enviados pelo cliente e decide a ação com base no conteúdo da requisição.
 *
 * @param arg Argumento adicional (não utilizado).
 * @param tpcb Estrutura do TCP (conexão atual).
 * @param p Buffer com os dados recebidos.
 * @param err Código de erro associado à conexão.
 * @return Código de erro (ERR_OK para sucesso).
 */
static err_t http_server_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if (!p) {
        // Finaliza a conexão se o cliente fecha a conexão.
        tcp_close(tpcb);
        tcp_recv(tpcb, NULL);
        return ERR_OK;
    }
    
    // Converte os dados recebidos para string
    char *data = (char*)p->payload;

    // Verifica o comando recebido na requisição
    if (strstr(data, "GET /?led=on")) {
        gpio_put(RED_LED_PIN, 1);  // Liga o LED
    } else if (strstr(data, "GET /?led=off")) {
        gpio_put(RED_LED_PIN, 0);  // Desliga o LED
    } else if (strstr(data, "GET /?music=play")) {
        playMusic();  // Chama a função para tocar música
    }

    // Envia a página HTML de controle como resposta
    tcp_write(tpcb, html_page, strlen(html_page), TCP_WRITE_FLAG_COPY);
    tcp_output(tpcb);

    // Libera o buffer recebido
    pbuf_free(p);
    return ERR_OK;
}

/**
 * Manipulador para aceitar conexões TCP.
 * Configura a função para tratar as requisições recebidas.
 *
 * @param arg Argumento adicional (não utilizado).
 * @param newpcb Estrutura do TCP para a nova conexão.
 * @param err Código de erro associado à conexão.
 * @return Código de erro (ERR_OK para sucesso).
 */
static err_t http_server_accept(void *arg, struct tcp_pcb *newpcb, err_t err) {
    // Configura o manipulador de recebimento para a nova conexão
    tcp_recv(newpcb, http_server_recv);
    return ERR_OK;
}

/**
 * Inicializa o servidor HTTP.
 * Configura o servidor para escutar na porta 80 e tratar conexões recebidas.
 */
void init_http_server() {
    // Cria um novo PCB para o TCP
    struct tcp_pcb *pcb = tcp_new();

    // Vincula o PCB à porta 80
    tcp_bind(pcb, IP_ADDR_ANY, 80);

    // Escuta por conexões no PCB
    pcb = tcp_listen(pcb);

    // Configura o manipulador para aceitar novas conexões
    tcp_accept(pcb, http_server_accept);
}

/**
 * Função principal do programa.
 * Inicializa o Wi-Fi, configura o LED e inicia o servidor HTTP.
 */
int main() {
    // Inicializa a interface de entrada/saída padrão
    stdio_init_all();

    // Configura o pino do LED como saída
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);

    // Inicializa o Wi-Fi
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true); // Liga o LED traseiro

    // Habilita o modo Station (STA) do Wi-Fi
    cyw43_arch_enable_sta_mode();

    printf("Connecting to Wi-Fi...\n");

    // Tenta conectar ao Wi-Fi com o SSID e senha fornecidos
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        printf("Failed to connect.\n");
        playErrorSound();

        return 1;
    } else {
        printf("Connected.\n");
        gpio_put(GREEN_LED_PIN, 1);  // Liga o LED verde
        playConfirmSound();
        sleep_ms(1000);
        gpio_put(GREEN_LED_PIN, 0);  // desliga o LED verde

        // Obtém o endereço IP do dispositivo
        uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
        printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

        // Inicializa o servidor HTTP
        init_http_server();

        // Loop principal para manter o programa rodando
        while (true) {
            cyw43_arch_poll();  // Processa eventos de rede
            sleep_ms(10000);    // Aguarda 10 segundos
        }
    }
}
