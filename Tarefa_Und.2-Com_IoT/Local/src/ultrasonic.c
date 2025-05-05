#include "ultrasonic.h"

float lastDistance;

void initHcsr04() {
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);

    // Inicia o sensor desligado
    gpio_put(TRIG_PIN, 0);

    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
}

float measureDistance() {
    // Enviar pulso de 10µs para o TRIG
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    const uint64_t timeout_us = 20000; // 20 ms
    absolute_time_t deadline = make_timeout_time_us(timeout_us);

    // Aguardar o pino ECHO ir para HIGH com timeout
    while (gpio_get(ECHO_PIN) == 0) {
        if (get_absolute_time() > deadline) {
            printf("Timeout esperando ECHO subir\n");
            return -1;
        }
    }
    absolute_time_t start = get_absolute_time();

    // Reconfigurar o deadline para aguardar o LOW
    deadline = make_timeout_time_us(timeout_us);
    while (gpio_get(ECHO_PIN) == 1) {
        if (get_absolute_time() > deadline) {
            printf("Timeout esperando ECHO descer\n");
            return -1;
        }
    }
    absolute_time_t end = get_absolute_time();

    // Calcular o tempo decorrido em microssegundos
    uint64_t duration = absolute_time_diff_us(start, end);

    // Converter o tempo em distância (velocidade do som = 343 m/s)
    float distance_cm = (duration * 0.0343) / 2;
    
    lastDistance = distance_cm;
}