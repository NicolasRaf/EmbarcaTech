#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "pico/stdlib.h"

// Definição dos pinos físicos
#define BUTTON_PIN 5
#define LED_PIN 11

// Variáveis simuladas para o botão e o LED
int simulatedButtonState = 0;
int simulatedLEDState = 0;

// Fila para compartilhar o estado do botão
QueueHandle_t buttonQueue;
// Semáforo binário para sinalizar o controle do LED
SemaphoreHandle_t ledSemaphore;

/**
 * @brief Tarefa 1: Lê o estado do botão e envia para a fila.
 * 
 * Esta tarefa é responsável por ler o estado do botão físico conectado
 * ao Raspberry Pi Pico e enviar o estado lido para uma fila. O estado
 * do botão é lido a cada 100ms para evitar problemas de debounce.
 *
 * @param pvParameters Parâmetros da tarefa (não utilizados).
 */
void vTaskReadButton(void *pvParameters) {
    // Ignora os parâmetros da tarefa
    (void) pvParameters;
    
    // Loop infinito para ler o estado do botão continuamente
    for (;;) {
        // Lê o estado do botão (LOW = pressionado, HIGH = solto)
        simulatedButtonState = gpio_get(BUTTON_PIN) == 0 ? 1 : 0;
        
        // Envia o estado do botão para a fila
        xQueueSend(buttonQueue, &simulatedButtonState, portMAX_DELAY);
        
        // Aguarda 100ms para evitar debounce
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/**
 * @brief Tarefa 2: Processa o estado do botão e sinaliza para o LED.
 * 
 * Esta tarefa é responsável por processar o estado do botão recebido pela
 * fila e sinalizar para o LED. Se o botão estiver pressionado, o LED
 * é acendido. Caso contrário, o LED é desligado.
 */
void vTaskProcessButton(void *pvParameters) {
    int buttonState;
    (void) pvParameters;
    for (;;) {
        if (xQueueReceive(buttonQueue, &buttonState, portMAX_DELAY) == pdPASS) {
            if (buttonState == 1) {
                xSemaphoreGive(ledSemaphore);
                printf("Botão pressionado. LED acendido.\n");
            } else {
                simulatedLEDState = 0;
                gpio_put(LED_PIN, 0); // Desliga o LED
                printf("Botão não pressionado. LED desligado.\n");
            }
        }
    }
}

/**
 * @brief Tarefa 3: Controle do LED
 * 
 * Esta tarefa é responsável por controlar o estado do LED.
 * 
 * A tarefa aguarda indefinidamente por um sinal de liberação do semáforo, que
 * é enviado pela tarefa vTaskProcessButton() quando o botão é pressionado.
 * Quando o semáforo é liberado, a tarefa acende o LED e imprime uma mensagem
 * no console.
 */
void vTaskControlLED(void *pvParameters) {
    (void) pvParameters; // Ignora os parâmetros da tarefa
    
    for (;;) {
        // Aguarda por um sinal de liberação do semáforo
        if (xSemaphoreTake(ledSemaphore, portMAX_DELAY) == pdTRUE) {
            // Acende o LED 
            simulatedLEDState = 1;
            gpio_put(LED_PIN, 1);
            printf("Botão pressionado. LED aceso.\n");
        }
    }
}

/**
 * @brief Função principal
 * 
 * Esta função é a entrada principal do programa e responsável por inicializar
 * os GPIOs, criar a fila e o semáforo e criar as tarefas.
 * 
 * A função inicializa os GPIOs para o botão e o LED, cria a fila que armazena
 * o estado do botão e o semáforo que controla o acesso ao LED. Em seguida,
 * cria as tarefas para ler o botão, processar o estado do botão e controlar o
 * LED. Por fim, inicia o escalonador do FreeRTOS.
 * 
 * @returns 0 em caso de sucesso, outro valor em caso de erro.
 */
int main(void) {
    stdio_init_all();

    // Inicializa GPIOs
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN); // Ativa pull-up interno

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0); // LED inicia desligado

    // Criação da fila e semáforo
    buttonQueue = xQueueCreate(10, sizeof(int));
    ledSemaphore = xSemaphoreCreateBinary();

    // Criação das tarefas
    xTaskCreate(vTaskReadButton, "Tarefa Botão", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(vTaskProcessButton, "Tarefa Processa Botão", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    xTaskCreate(vTaskControlLED, "Tarefa LED", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 3, NULL);

    // Inicia o escalonador do FreeRTOS
    vTaskStartScheduler();

    // Se o escalonador retornar, há erro
    for (;;) {
        printf("Erro ao iniciar o escalonador.\n");
    };
    return 0;
}