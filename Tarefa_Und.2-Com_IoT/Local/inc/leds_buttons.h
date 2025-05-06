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
extern char button1_state[12];
extern char button2_state[12];

// Protótipos de funções
void initGpioButtonLeds();
void update_button_states();
void blinkWarn();

#endif