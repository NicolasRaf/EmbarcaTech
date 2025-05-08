// joystick.h
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

extern int axisY;
extern int axisY;
extern char joystickDirection[10];

// limites do ADC
#define JOY_TH_HIGH 3000
#define JOY_TH_LOW  1000

void initJoystick();
void readJoystick(void);
static const char *calculateDirection(int x, int y);

#endif
