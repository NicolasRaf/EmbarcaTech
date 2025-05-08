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
}