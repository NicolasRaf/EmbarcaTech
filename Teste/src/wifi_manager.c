#include "wifi_manager.h"
#include "display.h"

void failureAction(char *debug){
    printf("%s", debug);
    gpio_put(RED_LED_PIN, 1);
    playErrorSound();
    sleep_ms(100);
    gpio_put(RED_LED_PIN, 0);
}

void succeedAction(char *debug){
    printf("%s", debug);
    gpio_put(GREEN_LED_PIN, 1);
    playConfirmSound();
    sleep_ms(1000);
    gpio_put(GREEN_LED_PIN, 0);
}


// Inicializa o Wi-Fi
int init_wifi() {
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    cyw43_arch_enable_sta_mode();
    return 0;
}

// Conecta ao Wi-Fi
int connect_wifi(const char *ssid, const char *password) {
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(ssid, password, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
        showText("Failed to connect.", 0, 0, 1, true);
        failureAction("Failed to connect.\n");
        return -1;
    }
    succeedAction("Connected.\n");
    return 0;
}

// Imprime o endereço IP
void print_ip_address() {
    // Obtém o endereço IP do estado da interface de rede
    uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);

    // Buffer para armazenar o IP formatado como string
    char formatedIP[32];

    // Formata o endereço IP em forma de string
    snprintf(formatedIP, sizeof(formatedIP), "%d.%d.%d.%d",
             ip_address[0], ip_address[1], ip_address[2], ip_address[3]);

    // Imprime o endereço IP no console
    printf(formatedIP);

    // Exibe o endereço IP no display OLED
    showText("IP address:", 0, 0, 1, true);
    showText(formatedIP, 0, 10, 1, false);
}
