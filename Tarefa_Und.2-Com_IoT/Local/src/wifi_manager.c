#include "wifi_manager.h"

char formatedIP[32];

// Inicializa o Wi-Fi
int initWifi() {

    printf("Initializing Wi-Fi...\n");
    if (cyw43_arch_init()) {
        printf("Wi-Fi init failed\n");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    cyw43_arch_enable_sta_mode();
    return 0;
}

// Conecta ao Wi-Fi
int connectWifi() {
    printf("Connecting to Wi-Fi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Failed to connect to Wi-Fi.\n");
        return -1;
    }
    printf("Connected.\n");
    
    if (netif_default)
    {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    return 0;
}
