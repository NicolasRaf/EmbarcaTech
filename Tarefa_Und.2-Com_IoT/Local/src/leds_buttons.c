#include "leds_buttons.h"

// Variáveis de estado
volatile int button1_state = 0;
volatile int button2_state = 0;

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
    button1_state = !gpio_get(BUTTON_A_PIN); // Invertido por causa do pull-up
    button2_state = !gpio_get(BUTTON_B_PIN);
}

void handle_led_request(const char *request) {
    // Mapeamento de ações para LEDs
    struct LedAction {
        const char *path;
        uint pin;
        bool state;
    };

    const struct LedAction actions[] = {
        {"/blue_on", LED_BLUE_PIN, true},
        {"/blue_off", LED_BLUE_PIN, false},
        {"/red_on", LED_RED_PIN, true},
        {"/red_off", LED_RED_PIN, false},
        {"/green_on", LED_GREEN_PIN, true},
        {"/green_off", LED_GREEN_PIN, false},
    };

    for (size_t i = 0; i < sizeof(actions)/sizeof(actions[0]); i++) {
        if (strstr(request, actions[i].path)) {
            gpio_put(actions[i].pin, actions[i].state);
            break;
        }
    }
}
