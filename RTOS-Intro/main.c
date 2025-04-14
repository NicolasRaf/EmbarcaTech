#include <stdio.h>
#include "pico/stdlib.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "pico/multicore.h"

// Semáforo e mutex para controle de acesso
SemaphoreHandle_t xSemaphore;
SemaphoreHandle_t xMutex;

// Definição dos pinos de LED e botões
#define LED_CORE0 13
#define BUTTON_CORE0 5
#define LED_CORE1 11
#define BUTTON_CORE1 6

// Função para configurar os GPIOs
void setup_gpio() {
    printf("[DEBUG] Configurando GPIOs...\n");
    // Configura os GPIOs para o núcleo 0
    gpio_init(LED_CORE0);
    gpio_set_dir(LED_CORE0, GPIO_OUT);
    gpio_init(BUTTON_CORE0);
    gpio_set_dir(BUTTON_CORE0, GPIO_IN);
    gpio_pull_up(BUTTON_CORE0);

    // Configura os GPIOs para o núcleo 1
    gpio_init(LED_CORE1);
    gpio_set_dir(LED_CORE1, GPIO_OUT);
    gpio_init(BUTTON_CORE1);
    gpio_set_dir(BUTTON_CORE1, GPIO_IN);
    gpio_pull_up(BUTTON_CORE1);
    printf("[DEBUG] GPIOs configurados com sucesso.\n");
}

// Tarefa para o núcleo 0: aciona LED13 com botão no pino 5
void taskCore0(void *pvParameters) {
    printf("[DEBUG] Tarefa Core 0 iniciada.\n");
    while (1) {
        if (gpio_get(BUTTON_CORE0) == 0) {
            printf("[DEBUG Core 0] Botão pressionado.\n");
            // Tenta adquirir o semáforo
            if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
                printf("[DEBUG Core 0] Semáforo adquirido.\n");
                // Tenta adquirir o mutex
                if (xMutex != NULL && xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                    printf("[DEBUG Core 0] Mutex adquirido. Ligando LED.\n");
                    // Liga o LED 13
                    gpio_put(LED_CORE0, 1);
                    printf("[Core 0] LED 13 ON\n");
                    // Aguarda 500ms
                    vTaskDelay(pdMS_TO_TICKS(500));
                    // Desliga o LED 13
                    gpio_put(LED_CORE0, 0);
                    printf("[Core 0] LED 13 OFF\n");
                    // Libera o mutex
                    xSemaphoreGive(xMutex);
                    printf("[DEBUG Core 0] Mutex liberado.\n");
                }
                // Libera o semáforo
                xSemaphoreGive(xSemaphore);
                printf("[DEBUG Core 0] Semáforo liberado.\n");
            }
        }
        // Aguarda 100ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa para o núcleo 1: aciona LED11 com botão no pino 6
void taskCore1(void *pvParameters) {
    printf("[DEBUG] Tarefa Core 1 iniciada.\n");
    while (1) {
        if (gpio_get(BUTTON_CORE1) == 0) {
            printf("[DEBUG Core 1] Botão pressionado.\n");
            // Tenta adquirir o mutex
            if (xMutex != NULL && xSemaphoreTake(xMutex, portMAX_DELAY) == pdTRUE) {
                printf("[DEBUG Core 1] Mutex adquirido. Ligando LED.\n");
                // Liga o LED 11
                gpio_put(LED_CORE1, 1);
                printf("[Core 1] LED 11 ON\n");
                // Aguarda 500ms
                vTaskDelay(pdMS_TO_TICKS(500));
                // Desliga o LED 11
                gpio_put(LED_CORE1, 0);
                printf("[Core 1] LED 11 OFF\n");
                // Libera o mutex
                xSemaphoreGive(xMutex);
                printf("[DEBUG Core 1] Mutex liberado.\n");
            }
        }
        // Aguarda 100ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Inicializa o núcleo 1 com sua tarefa
void core1_main() {
    printf("[DEBUG] Iniciando tarefa no núcleo 1...\n");
    xTaskCreate(taskCore1, "Task Core 1", 1024, NULL, 1, NULL);
    vTaskStartScheduler();
    printf("[DEBUG] Escalonador núcleo 1 iniciado.\n");
}

int main() {
    stdio_init_all();
    sleep_ms(1000); // Aguarda 1 segundo para garantir que o USB CDC seja reconhecido
    printf("[DEBUG] Inicializando sistema...\n");
    setup_gpio(); 

    // Cria o semáforo e mutex
    xSemaphore = xSemaphoreCreateBinary();
    xMutex = xSemaphoreCreateMutex();
    printf("[DEBUG] Semáforo e mutex criados.\n");

    // Libera o semáforo inicialmente
    if (xSemaphore != NULL) {
        xSemaphoreGive(xSemaphore);
        printf("[DEBUG] Semáforo liberado inicialmente.\n");
    }

    // Lança o núcleo 1 com sua tarefa
    multicore_launch_core1(core1_main);
    printf("[DEBUG] Núcleo 1 lançado.\n");

    // Cria a tarefa para o núcleo 0
    xTaskCreate(taskCore0, "Task Core 0", 1024, NULL, 1, NULL);
    printf("[DEBUG] Tarefa núcleo 0 criada.\n");

    // Inicia o escalonador
    vTaskStartScheduler();
    printf("[DEBUG] Escalonador núcleo 0 iniciado.\n");

    // Loop infinito
    while (true) {
        tight_loop_contents();
    }
}

