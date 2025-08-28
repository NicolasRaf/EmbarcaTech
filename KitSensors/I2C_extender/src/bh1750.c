#include "bh1750.h"

// --- Constantes do BH1750 ---
const uint8_t BH1750_ADDR = 0x23;
const uint8_t BH1750_POWER_ON = 0x01;
const uint8_t BH1750_CONT_HIGH_RES_MODE = 0x10;

// Configuração do I2C 0 da placa e do sensor
void bh1750_init() {
    printf("Configurando BH1750 no I2C0...\n");

    // Configura o sensor BH1750
    bh1750_set_mode(BH1750_POWER_ON);
    sleep_ms(10);
    bh1750_set_mode(BH1750_CONT_HIGH_RES_MODE);
    sleep_ms(180);

    printf("BH1750 configurado e pronto para leitura.\n");
}

// Funções para leitura e configuração do sensor
void bh1750_set_mode(uint8_t mode) {
    i2c_write_blocking(i2c0, BH1750_ADDR, &mode, 1, false);
}

// Leitura dos dados do sensor
void bh1750_read_lux(float *lux) {
    uint8_t buffer[2];
    i2c_read_blocking(i2c0, BH1750_ADDR, buffer, 2, false);
    uint16_t raw = (buffer[0] << 8) | buffer[1];
    *lux = raw / 1.2f;
}