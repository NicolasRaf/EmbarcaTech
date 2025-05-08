#include "leds_buttons.h"

// Variáveis de estado
char buttonAState[12] = "";
char buttonBState[12] = "";

/**
 * Inicializa os LEDs e botões com GPIOs.
 *
 * Configura os LEDs como saídas e os botões como entradas.
 * Os LEDs são inicializados desligados e os botões com pull-up.
 */

void initGpioButtonLeds() {
    // Configuração dos LEDs
    const uint ledPins[] = {LED_BLUE_PIN, LED_RED_PIN, LED_GREEN_PIN};
    for (int i = 0; i < 3; i++) {
        gpio_init(ledPins[i]);
        gpio_set_dir(ledPins[i], GPIO_OUT);
        gpio_put(ledPins[i], 0); // Apaga o LED
    }

    // Configuração dos botões
    const uint buttonPins[] = {BUTTON_A_PIN, BUTTON_B_PIN};
    for (int i = 0; i < 2; i++) {
        gpio_init(buttonPins[i]);
        gpio_set_dir(buttonPins[i], GPIO_IN);
        gpio_pull_up(buttonPins[i]); // Habilita pull-up
    }
}

/**
 * Atualiza as variáveis de estado dos botões com os seus estados
 * atuais.
 */
void updateButtonStates() {
    strcpy(buttonAState, gpio_get(BUTTON_A_PIN) ? "Solto" : "Pressionado");
    strcpy(buttonBState, gpio_get(BUTTON_B_PIN) ? "Solto" : "Pressionado");
}

/**
 * Pisca os LEDs vermelho e verde em forma de alerta.
 *
 * Alterna entre o estado ligado e desligado dos LEDs
 * vermelho e verde, criando um efeito de piscada.
 */
void blinkWarn() {
    static bool state = false;
    gpio_put(LED_RED_PIN, state);
    gpio_put(LED_GREEN_PIN, state);
    state = !state;
}

/**
 * Desliga todos os LEDs.
 *
 * Define o estado dos pinos dos LEDs azul, vermelho e verde como 
 * baixo (0), apagando assim todos os LEDs.
 */

void turnOffLeds() {
    gpio_put(LED_BLUE_PIN, 0);
    gpio_put(LED_RED_PIN, 0);
    gpio_put(LED_GREEN_PIN, 0);
}


