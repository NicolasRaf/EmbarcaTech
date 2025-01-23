#include <stdio.h>
#include "pico/stdlib.h"
#include "init.h"

// Loop principal
void main_loop() {
    cyw43_arch_poll();
    sleep_ms(100);
}

// Função principal
int main() {
    initializeSystem();

    if (init_wifi() != 0) return -1;
    if (connect_wifi(WIFI_SSID, WIFI_PASSWORD) != 0) return -1;
    clearScreen();

    print_ip_address();
    initHttpServer();

    while (1){
        main_loop();
    }
    
    return 0;
}
