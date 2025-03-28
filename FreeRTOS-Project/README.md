# Controle de LED com Botão utilizando FreeRTOS e Raspberry Pi Pico W

Este projeto utiliza FreeRTOS para gerenciar as tarefas de controle de um LED com um botão conectado ao Raspberry Pi Pico W. O sistema lê o estado do botão e acende um LED quando o botão é pressionado. O código demonstra a utilização de tarefas, filas e semáforos do FreeRTOS para controlar o comportamento do sistema.

## Componentes Utilizados

- **Raspberry Pi Pico W**: Microcontrolador responsável pelo controle do LED e do botão.
- **LED**: Utilizado para indicar o estado do botão (aceso quando pressionado).
- **Botão**: Componente de entrada para acionar o LED.
- **FreeRTOS**: Sistema operacional de tempo real utilizado para gerenciar as tarefas.

## Descrição do Funcionamento

O código consiste em três tarefas principais:

1. **vTaskReadButton**: Lê o estado do botão a cada 100ms e envia o estado para uma fila.
2. **vTaskProcessButton**: Processa o estado do botão. Se o botão for pressionado, a tarefa libera um semáforo para acender o LED. Caso contrário, o LED é desligado.
3. **vTaskControlLED**: Controla o estado do LED. A tarefa aguarda pela liberação do semáforo e acende o LED quando o semáforo é liberado.

## Funcionamento do Sistema

1. **Leitura do Botão**: A tarefa `vTaskReadButton` monitora o estado do botão a cada 100ms. Quando o botão é pressionado, o estado é enviado para a fila.
2. **Processamento do Estado do Botão**: A tarefa `vTaskProcessButton` verifica o estado do botão. Quando o botão é pressionado, a tarefa libera o semáforo para a tarefa `vTaskControlLED`, indicando que o LED deve ser aceso.
3. **Controle do LED**: A tarefa `vTaskControlLED` aguarda a liberação do semáforo e, ao recebê-lo, acende o LED.

## Dependências

Este código utiliza a biblioteca **FreeRTOS** para gerenciar o sistema de tarefas, filas e semáforos. A biblioteca FreeRTOS é incluída no ambiente de desenvolvimento para o Raspberry Pi Pico W.

Além disso, a biblioteca `pico/stdlib.h` é utilizada para controlar os pinos GPIO do Raspberry Pi Pico W.

## Considerações

- O código utiliza um pull-up interno no botão, ou seja, o botão deve ser conectado ao GPIO 5.
- O LED está conectado ao GPIO 11 do Raspberry Pi Pico W e será aceso quando o botão for pressionado.
