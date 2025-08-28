#ifndef SERVOR_H
#define SERVOR_H

#include <stdio.h> 
#include "hardware/pwm.h" 
#include "hardware/clocks.h" 
#include "hardware/gpio.h" 

// Frequência do sinal PWM para servos (geralmente 50Hz, ou um período de 20ms)
#define PWM_PERIOD_MS 20

#define SERVO_PIN 28 // Pino GPIO para o SG90

// Largura do pulso em microssegundos (us) para o SG90
#define SERVO_MIN_PULSE_US 500 
#define SERVO_MAX_PULSE_US 2500 

extern int servoState; // Variável externa para armazenar o estado do servo

void servo_init();
void servo_set_angle(uint angle);

#endif // SERVOR_H