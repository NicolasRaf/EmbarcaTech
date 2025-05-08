#include "wifi_manager.h"

bool wifiConnected = false;

/**
 * Inicializa o Wi-Fi.
 *
 * @return 0 em caso de sucesso, -1 caso contr rio.
 */
int initWifi() {

    printf("Initializing Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    // habilita o LED de Wi-Fi (se estiver presente na placa)
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    // habilita o modo STA (Station)
    cyw43_arch_enable_sta_mode();
    return 0;
}

/**
 * Conecta ao Wi-Fi usando credenciais predefinidas.
 *
 * Tenta estabelecer uma conexão Wi-Fi de forma assíncrona.
 * O estado da conexão é verificado periodicamente até que a
 * conexão seja estabelecida ou ocorra um timeout.
 *
 * @return 0 em caso de sucesso, -1 em caso de falha.
 */
int connectWifi() {
    printf("Connecting to Wi-Fi...\n");

    int timeout_ms = 20000;  // Tempo máximo de espera para conexão
    int interval_ms = 500;   // Intervalo entre verificações de status
    int elapsed = 0;         // Tempo decorrido

    // Inicia conexão Wi-Fi de forma assíncrona
    if (cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
        printf("Failed to start Wi-Fi connection.\n");
        return -1;
    }

    // Verifica o status da conexão até timeout
    while (elapsed < timeout_ms) {
        cyw43_arch_poll();  // Atualiza o estado da conexão
        blinkWarn();        // Pisca LEDs para indicar tentativa de conexão

        // Verifica o status do link
        int status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
        if (status == CYW43_LINK_UP) {
            wifiConnected = true;
            printf("Connected to Wi-Fi.\n");
            gpio_put(LED_RED_PIN, 0);   // Apaga LED vermelho
            gpio_put(LED_GREEN_PIN, 1); // Acende LED verde

            // Se o IP estiver disponível, imprime o endereço
            if (netif_default) {
                printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
            }
            sleep_ms(500);
            gpio_put(LED_GREEN_PIN, 0); // Apaga LED verde após notificação

            return 0; // Conexão bem-sucedida
        }
        sleep_ms(interval_ms); // Aguarda intervalo antes de nova verificação
        elapsed += interval_ms; // Atualiza tempo decorrido
    }

    // Conexão falhou após timeout
    printf("Failed to connect to Wi-Fi.\n");
    gpio_put(LED_GREEN_PIN, 0); // Garante que LED verde está apagado
    gpio_put(LED_RED_PIN, 1);   // Acende LED vermelho para indicar falha
    return -1;
}
