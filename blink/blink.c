#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <stdio.h>

// Variáveis simuladas para o botão e o LED
int simulatedButtonState = 0;
int simulatedLEDState = 0;

// Fila para compartilhar o estado do botão
QueueHandle_t buttonQueue;
// Semáforo binário para sinalizar o controle do LED
SemaphoreHandle_t ledSemaphore;

// Tarefa 1: Leitura do Botão
void vTaskReadButton(void *pvParameters) {
    (void) pvParameters;
    for (;;) {
        // Simula a leitura do botão (alternando o estado)
        simulatedButtonState = !simulatedButtonState; // 0 alterna para 1 e vice-versa
        // Envia o estado do botão para a fila
        xQueueSend(buttonQueue, &simulatedButtonState, portMAX_DELAY);
        // Aguarda 100ms
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Tarefa 2: Processamento do Botão
void vTaskProcessButton(void *pvParameters) {
    int buttonState;
    (void) pvParameters;
    for (;;) {
        // Aguarda a recepção do estado do botão
        if (xQueueReceive(buttonQueue, &buttonState, portMAX_DELAY) == pdPASS) {
            // Se o botão estiver pressionado (valor 1)
            if (buttonState == 1) {
                // Sinaliza para a tarefa do LED acender
                xSemaphoreGive(ledSemaphore);
            } else {
                // Se não estiver pressionado, garante que o LED esteja desligado
                simulatedLEDState = 0;
                printf("Botão não pressionado. LED desligado.\n");
            }
        }
    }
}

// Tarefa 3: Controle do LED
void vTaskControlLED(void *pvParameters) {
    (void) pvParameters;
    for (;;) {
        // Aguarda o semáforo ser liberado pela Tarefa 2
        if (xSemaphoreTake(ledSemaphore, portMAX_DELAY) == pdTRUE) {
            // Acende o LED
            simulatedLEDState = 1;
            printf("Botão pressionado. LED aceso.\n");
        }
    }
}

int main(void) {
    // Criação da fila para compartilhar o estado do botão (capacidade para 10 elementos)
    buttonQueue = xQueueCreate(10, sizeof(int));
    // Criação do semáforo binário para o LED
    ledSemaphore = xSemaphoreCreateBinary();

    // Criação das tarefas com diferentes prioridades
    xTaskCreate(vTaskReadButton, "Tarefa Botão", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vTaskProcessButton, "Tarefa Processa Botão", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTaskControlLED, "Tarefa LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();

    // Se o escalonador retornar, há erro (não deveria acontecer)
    for (;;);
    return 0;
}
