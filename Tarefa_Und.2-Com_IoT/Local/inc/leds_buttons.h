#ifndef LEDS_BUTTONS_H
#define LEDS_BUTTONS_H

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "lwip/tcp.h"


// Definição dos pinos
#define LED_BLUE_PIN 12
#define LED_RED_PIN 13
#define LED_GREEN_PIN 11
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

// Variáveis compartilhadas
extern struct tcp_pcb *current_client;
extern volatile int button1_state;
extern volatile int button2_state;

// Protótipos de funções
void initGpioButtonLeds();
void update_button_states();
void handle_led_request(const char *request);

#endif