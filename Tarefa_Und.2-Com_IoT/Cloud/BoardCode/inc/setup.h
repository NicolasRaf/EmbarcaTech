#ifndef SETUP_H
#define SETUP_H

// Includes do C
#include <stdio.h> 
#include <stdlib.h>
#include "string.h"

// Includes do Pico
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"

// Includes do projeto
#include "wifi_manager.h"
#include "ultrasonic.h"
#include "leds_buttons.h"
#include "joystick.h"
#include "server.h"

// Includes do lwIP
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/netif.h" 
#include "lwipopts.h"

#define TRIG_PIN 17
#define ECHO_PIN 16

void initializeSystem();
void initInterrupts();

#endif // SETUP_H
