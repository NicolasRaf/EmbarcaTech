#ifndef ANALOGIC_H
#define ANALOGIC_H

#include <stdint.h>
#include <string.h>

/**
 * Preenche `dir` com uma string representando a direção da rosa dos ventos
 * com base nos valores de ADC de X e Y.
 *
 * @param x Valor lido do eixo X (ADC)
 * @param y Valor lido do eixo Y (ADC)
 * @param dir Buffer para receber a string. Deve ter pelo menos 20 caracteres.
 */
void selectDirectionWindRose(uint16_t x, uint16_t y, char *dir);

#endif // ANALOGIC_H