// joystick.h
#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

extern int x_value;
extern int y_value;
extern char directionWindRose[3];

void read_joystick(void);
void calculate_direction(void);

#endif
