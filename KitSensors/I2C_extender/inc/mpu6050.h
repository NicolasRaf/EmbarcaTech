#ifndef MPU6050_H
#define MPU6050_H

#include <math.h>
#include <stdint.h>
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define MPU6050_ADDR 0x68 // Endereço I2C do MPU6050
#define ACCEL_SENSITIVITY 16384.0f // Sensibilidade do acelerometro
#define SDA_PIN 0
#define SCL_PIN 1

void mpu6050_init(void);
void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az);
float mpu6050_get_pitch(int16_t ax, int16_t ay, int16_t az);
float mpu6050_get_roll(int16_t ay, int16_t az);
long map(long x, long in_min, long in_max, long out_min, long out_max);

#endif