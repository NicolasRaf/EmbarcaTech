#include "joystick.h"

void initJoystick() {
    adc_init();
    adc_gpio_init(26); // Configure GPIO pin for X-axis
    adc_gpio_init(27); // Configure GPIO pin for Y-axis
}

// buffer global para direção: pode armazenar até 2 chars + '\0'
char directionWindRose[10] = "Centro";
int x_value = 0;
int y_value = 0;

// tabela: primeiro índice é eixo Y (-1=S,0=C,1=N), segundo é X (-1=W,0=C,1=E)
static const char * const dir_map[3][3] = {
    { "Sudoeste", "Sul",  "Sudeste" },
    { "Oeste",  "Centro",  "Leste"  },
    { "Noroeste", "Norte",  "Nordeste" }
};

/**
 * Converte valores bruto do ADC em índices -1,0,1
 *   x < JOY_TH_LOW  → -1
 *   JOY_TH_LOW ≤ x ≤ JOY_TH_HIGH → 0
 *   x > JOY_TH_HIGH → +1
 */
static inline int axis_index(int v) {
    if (v < JOY_TH_LOW)  return -1;
    if (v > JOY_TH_HIGH) return +1;
    return 0;
}

/**
 * Retorna ponteiro para string literal da direção correspondente.
 */
static const char *calculate_direction(int x, int y) {
    int ix = axis_index(x) + 1;
    int iy = axis_index(y) + 1;
    return dir_map[iy][ix];
}

/**
 * Lê o joystick (ADC 0 = X, ADC 1 = Y) e atualiza globals x_value, y_value
 * e directionWindRose.
 */
void read_joystick(void) {
    adc_select_input(0);
    y_value = adc_read();
    adc_select_input(1);
    x_value = adc_read();

    const char *d = calculate_direction(x_value, y_value);
    // cópia segura para o buffer global
    strncpy(directionWindRose, d, sizeof(directionWindRose)-1);
    directionWindRose[sizeof(directionWindRose)-1] = '\0';
}
