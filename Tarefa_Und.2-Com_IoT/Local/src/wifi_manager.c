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
    int connectWifi() {
        printf("Connecting to Wi-Fi...\n");

        int timeout_ms = 20000;
        int interval_ms = 500;
        int elapsed = 0;

        if (cyw43_arch_wifi_connect_async(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK)) {
            printf("Failed to start Wi-Fi connection.\n");
            return -1;
        }

        while (elapsed < timeout_ms) {
            cyw43_arch_poll();
            blinkWarn();

            int status = cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA);
            if (status == CYW43_LINK_UP) {
                printf("Connected to Wi-Fi.\n");
                gpio_put(LED_RED_PIN, 0);
                gpio_put(LED_GREEN_PIN, 1);

                if (netif_default) {
                    printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
                }
                sleep_ms(500);
                gpio_put(LED_GREEN_PIN, 0);

                return 0;
            }
            sleep_ms(interval_ms);
            elapsed += interval_ms;
        }

        printf("Failed to connect to Wi-Fi.\n");
        gpio_put(LED_GREEN_PIN, 0);
        gpio_put(LED_RED_PIN, 1);
        return -1;
    }
