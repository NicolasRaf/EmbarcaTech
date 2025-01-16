#include <stdio.h>
#include "pico/stdlib.h"
#include "wifi_manager.h"
#include "server.h"
#include "music.h"


void init_http_server() {
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_server_accept);
}

// Inicializa os pinos GPIO
void init_gpio() {
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
}

// Loop principal
void main_loop() {
    while (true) {
        cyw43_arch_poll();
        sleep_ms(10000);
    }
}

// Função principal
int main() {
    stdio_init_all();
    init_gpio();

    if (init_wifi() != 0) {
        return -1;
    }

    if (connect_wifi(WIFI_SSID, WIFI_PASSWORD) != 0) {
        return -1;
    }

    print_ip_address();
    init_http_server();
    main_loop();

    return 0;
}

