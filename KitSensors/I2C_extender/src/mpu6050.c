#include "mpu6050.h"

void mpu6050_init(void) {
    // Endereços dos registradores
    const uint8_t REG_PWR_MGMT_1 = 0x6B;
    const uint8_t REG_ACCEL_CONFIG = 0x1C;

    // Tira o sensor do modo de suspensão (sleep mode)
    uint8_t power_cmd[2] = {REG_PWR_MGMT_1, 0x00};
    i2c_write_blocking(i2c0, MPU6050_ADDR, power_cmd, 2, false);

    // Configura a sensibilidade do acelerômetro para ±2g (o valor padrão)
    // Isso torna o código mais robusto, garantindo a escala correta.
    uint8_t accel_cmd[2] = {REG_ACCEL_CONFIG, 0x00};
    i2c_write_blocking(i2c0, MPU6050_ADDR, accel_cmd, 2, false);
}

// Função para ler os dados brutos do acelerômetro
void mpu6050_read_raw(int16_t *ax, int16_t *ay, int16_t *az) {
    const uint8_t REG_ACCEL_XOUT_H = 0x3B;
    uint8_t buffer[6];

    // Aponta para o primeiro registrador do acelerômetro (ACCEL_XOUT_H)
    i2c_write_blocking(i2c0, MPU6050_ADDR, &REG_ACCEL_XOUT_H, 1, true); // true = no-stop
    
    // Lê os 6 bytes de dados (X, Y, Z, cada um com 2 bytes)
    i2c_read_blocking(i2c0, MPU6050_ADDR, buffer, 6, false); // false = stop

    // Combina os bytes alto (H) e baixo (L) para formar valores de 16 bits
    *ax = (buffer[0] << 8) | buffer[1];
    *ay = (buffer[2] << 8) | buffer[3];
    *az = (buffer[4] << 8) | buffer[5];
}

// Função para calcular o ângulo de inclinação no eixo X
float mpu6050_get_pitch(int16_t ax, int16_t ay, int16_t az) {
    // Converte os valores brutos para G's
    float ax_g = ax / ACCEL_SENSITIVITY;
    float ay_g = ay / ACCEL_SENSITIVITY;
    float az_g = az / ACCEL_SENSITIVITY;

    // Calcula o ângulo em radianos e converte para graus
    return atan2(ay_g, sqrt(ay_g * ay_g + az_g * az_g)) * (180.0 / M_PI);
}

// Função para calcular o ângulo de inclinação no eixo Y
float mpu6050_get_roll(int16_t ay, int16_t az) {
    // Converte os valores brutos para G's
    float ay_g = ay / ACCEL_SENSITIVITY;
    float az_g = az / ACCEL_SENSITIVITY;
    
    return atan2(ay_g, az_g) * (180.0 / M_PI);
}

// Função para mapear um valor de um intervalo para outro
long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}