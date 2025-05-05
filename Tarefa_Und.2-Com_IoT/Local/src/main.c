#include "setup.h"
#include "server.h"

int main() {
    initializeSystem();

    sleep_ms(200);
    connectWifi();

    start_server();  // função definida em server.c
    while (true) {
        cyw43_arch_poll();
        sleep_ms(100);
    }

    cyw43_arch_deinit();
    return 0;
}