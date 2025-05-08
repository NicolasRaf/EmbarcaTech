#include "joystick.h"

// Inicializa o joystick (ADC 0 = X, ADC 1 = Y)
void initJoystick() {
    adc_init();
    adc_gpio_init(26); // Configure GPIO pin for X-axis
    adc_gpio_init(27); // Configure GPIO pin for Y-axis
}

char joystickDirection[10] = "Centro"; // Direção do joystick
int axisY = 0;
int axisX = 0;

// tabela: primeiro índice é eixo Y (-1=S,0=C,1=N), segundo é X (-1=W,0=C,1=E)
static const char * const dirMap[3][3] = {
    { "Sudoeste", "Sul",  "Sudeste" },
    { "Oeste",  "Centro",  "Leste"  },
    { "Noroeste", "Norte",  "Nordeste" }
};

/**
 * Converte valores brutos do ADC em índices -1,0,1 para lookup em tabela.
 *   x < JOY_TH_LOW  → -1
 *   JOY_TH_LOW ≤ x ≤ JOY_TH_HIGH → 0
 *   x > JOY_TH_HIGH → +1
 *
 * @param v Valor bruto lido do ADC
 * @return -1, 0 ou +1
 */
static inline int AxisIndex(int v) {
    if (v < JOY_TH_LOW)  return -1; // Se o valor for menor que o limite inferior, retorna -1
    if (v > JOY_TH_HIGH) return +1; // Se o valor for maior que o limite superior, retorna +1
    return 0;
}

/**
 * Retorna ponteiro para string literal da direção correspondente.
 *
 * @param x Valor bruto do eixo X
 * @param y Valor bruto do eixo Y
 * @return Ponteiro para string literal da direção
 */
static const char *calculateDirection(int x, int y) {
    int ix = AxisIndex(x) + 1;
    int iy = AxisIndex(y) + 1;
    return dirMap[iy][ix];
}

/**
 * Lê o joystick (ADC 0 = X, ADC 1 = Y) e atualiza globals axisY, axisX
 * e joystickDirection.
 */
void readJoystick(void) {
    adc_select_input(0);
    axisX = adc_read();
    adc_select_input(1);
    axisY = adc_read();

    const char *d = calculateDirection(axisY, axisX);
    // cópia segura para o buffer global
    strncpy(joystickDirection, d, sizeof(joystickDirection)-1);
    joystickDirection[sizeof(joystickDirection)-1] = '\0';
}
