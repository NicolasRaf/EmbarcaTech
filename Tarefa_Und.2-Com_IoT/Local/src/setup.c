#include "setup.h"

void initializeSystem() {
    stdio_init_all();

    adc_init();
    adc_gpio_init(26); // X-axis
    adc_gpio_init(27); // Y-axis

    initWifi();
    initGpioButtonLeds();
    initHcsr04();
    initInterrupts();
}


void initInterrupts() {
    // gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_RISE, true, buttons_callback);
    // gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_RISE, true, buttons_callback);
}
