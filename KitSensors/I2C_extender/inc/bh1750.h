#ifndef BH1750_I2C_H
#define BH1750_I2C_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

void bh1750_init();
void bh1750_set_mode(uint8_t mode);
void bh1750_read_lux(float *lux);

#endif // BH1750_I2C_H