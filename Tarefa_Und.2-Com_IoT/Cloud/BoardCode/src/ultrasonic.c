#include "ultrasonic.h"

float lastDistance; // Buffer para armazenar os últimos valores lidos

/**
 * Inicializa o sensor ultrassônico HC-SR04.
 *
 * Configura o pino TRIG como saída e o pino ECHO como entrada.
 * O pino TRIG é inicializado em estado baixo, mantendo o sensor desligado.
 */

void initHcsr04() {
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);

    // Inicia o sensor desligado
    gpio_put(TRIG_PIN, 0);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

/**
 * Mede a distância em centímetros usando o sensor ultrassônico HC-SR04.
 *
 * Envia um pulso de 10 microsegundos no pino TRIG, e espera o pino ECHO
 * subir e descer. Calcula a distância em centímetros com base na duração
 * do pulso.
 *
 * @return Distância em centímetros, ou -1.0 se a leitura falhar.
 */
float measureDistance() {
    const uint64_t timeout_us = 38000; // ~38ms timeout (aprox. 6.5 metros, considerando o tempo de ida e volta do som)

    // Garante um pulso limpo no TRIG
    gpio_put(TRIG_PIN, 0);
    sleep_us(2);
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    // Espera o ECHO ir para HIGH com timeout
    absolute_time_t deadline = make_timeout_time_us(timeout_us);
    while (gpio_get(ECHO_PIN) == 0) {
        if (get_absolute_time() > deadline) {
            printf("Timeout esperando ECHO subir\n");
            return -1.0;
        }
    }
    absolute_time_t start = get_absolute_time();

    // Espera o ECHO ir para LOW com novo timeout
    deadline = make_timeout_time_us(timeout_us);
    while (gpio_get(ECHO_PIN) == 1) {
        if (get_absolute_time() > deadline) {
            printf("Timeout esperando ECHO descer\n");
            return -1.0;
        }
    }
    absolute_time_t end = get_absolute_time();

    // Calcula duração do pulso em microsegundos
    uint64_t duration_us = absolute_time_diff_us(start, end);

    // Calcula a distância em centímetros
    float distance_cm = (duration_us * 0.0343f) / 2.0f;

    // Filtra leituras fora dos limites usuais do sensor
    if (distance_cm < 1.0f || distance_cm > 400.0f) {
        printf("Leitura inválida: %.2f cm\n", distance_cm);
        return -1.0f;
    }

    lastDistance = distance_cm;
    return distance_cm;
}
