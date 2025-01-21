#include <stdio.h>
#include "pico/stdlib.h"
#include "wifi_manager.h"
#include "server.h"
#include "display.h"


void initHttpServer() {
    struct tcp_pcb *pcb = tcp_new();
    tcp_bind(pcb, IP_ADDR_ANY, 80);
    pcb = tcp_listen(pcb);
    tcp_accept(pcb, http_server_accept);
}

// Inicializa os pinos GPIO
void initGpio() {
    gpio_init(RED_LED_PIN);
    gpio_set_dir(RED_LED_PIN, GPIO_OUT);

    gpio_init(GREEN_LED_PIN);
    gpio_set_dir(GREEN_LED_PIN, GPIO_OUT);
}

void initI2C() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
}

// Loop principal
void main_loop() {
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
    }
}

// Função principal
int main() {
    stdio_init_all();
    initGpio();
    initI2C();
    initializeDisplay();

    if (init_wifi() != 0) return -1;
    if (connect_wifi(WIFI_SSID, WIFI_PASSWORD) != 0) return -1;

    print_ip_address();
    initHttpServer();
    main_loop();

    return 0;
}

