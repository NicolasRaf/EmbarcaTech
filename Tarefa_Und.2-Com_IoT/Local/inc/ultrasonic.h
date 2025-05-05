#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "leds_buttons.h"

#define TRIG_PIN 17
#define ECHO_PIN 16

extern float lastDistance; // Buffer para armazenar os últimos valores lidos

void initHcsr04();
float measureDistance();

#endif // ULTRASONIC_H
