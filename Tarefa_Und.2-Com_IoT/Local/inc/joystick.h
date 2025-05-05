// joystick.h
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

extern int x_value;
extern int y_value;
extern char directionWindRose[10];

void read_joystick(void);
static const char *calculate_direction(int x, int y);

#endif
