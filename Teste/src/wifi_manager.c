#include "wifi_manager.h"

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
        failureAction("Failed to connect.\n");
        return -1;
    }
    succeedAction("Connected.\n");
    return 0;
}

// Imprime o endereço IP
void print_ip_address() {
    uint8_t *ip_address = (uint8_t*)&(cyw43_state.netif[0].ip_addr.addr);
    printf("IP address %d.%d.%d.%d\n", ip_address[0], ip_address[1], ip_address[2], ip_address[3]);
}