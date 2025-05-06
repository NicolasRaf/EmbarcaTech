#include "leds_buttons.h"

// Variáveis de estado
char button1_state[12] = "";
char button2_state[12] = "";

void initGpioButtonLeds() {
    // Configuração dos LEDs
    const uint led_pins[] = {LED_BLUE_PIN, LED_RED_PIN, LED_GREEN_PIN};
    for (int i = 0; i < 3; i++) {
        gpio_init(led_pins[i]);
        gpio_set_dir(led_pins[i], GPIO_OUT);
        gpio_put(led_pins[i], 0); // Apaga o LED
    }

    // Configuração dos botões
    const uint button_pins[] = {BUTTON_A_PIN, BUTTON_B_PIN};
    for (int i = 0; i < 2; i++) {
        gpio_init(button_pins[i]);
        gpio_set_dir(button_pins[i], GPIO_IN);
        gpio_pull_up(button_pins[i]); // Habilita pull-up
    }
}

void update_button_states() {
    strcpy(button1_state, gpio_get(BUTTON_A_PIN) ? "Solto" : "Pressionado");
    strcpy(button2_state, gpio_get(BUTTON_B_PIN) ? "Solto" : "Pressionado");
}

void blinkWarn() {
    static bool state = false;
    gpio_put(LED_RED_PIN, state);
    gpio_put(LED_GREEN_PIN, state);
    state = !state;
}
