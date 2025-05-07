#include "setup.h"

void initializeSystem() {
    stdio_init_all();

    initJoystick();
    initWifi();
    initGpioButtonLeds();
    initHcsr04();
}

void initInterrupts() {
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL, resetConnection, NULL);
}