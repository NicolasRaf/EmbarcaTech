#include "setup.h"

/**
 * Inicializa o sistema com todas as configura es
 * necess rio para o funcionamento do projeto.
 *
 * Chama todas as fun es de inicializa o de
 * perif ricos e rotinas.
 */
void initializeSystem() {
    stdio_init_all(); // Inicializa a sa da padr o

    initJoystick(); // Inicializa o joystick
    initWifi(); // Inicializa o Wi-Fi
    initGpioButtonLeds(); // Inicializa os LEDs e bot es com GPIOs
    initHcsr04(); // Inicializa o sensor ultrass nico
    initInterrupts(); // Configura as interrup es do sistema
}

void resetConnection() {
    printf("Resetando conexão...\n");
    blinkWarn();

    sleep_ms(200);
    close_tcp_connection();
    retries = 0;
    create_tcp_connection();
    turnOffLeds();
}

/**
 * Configura as interrup es do sistema.
 *
 * Habilita interrupções para os botões e configura
 * as funções de callback para elas.
 */
void initInterrupts() {
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, resetConnection);
}

