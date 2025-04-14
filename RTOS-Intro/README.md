# Projeto: RTOS Multicore com Raspberry Pi Pico W

Este projeto demonstra o uso de **FreeRTOS** em um ambiente multicore com o **Raspberry Pi Pico W**, utilizando **semaphores**, **mutexes**, tarefas separadas por núcleo e controle de GPIOs com botões e LEDs.

## Requisitos

- Raspberry Pi Pico W
- 2 LEDs
- 2 botões
- Jumpers e resistores (pull-down se preferir)
- FreeRTOS Portado para Pico (Pico SDK com FreeRTOS)
- Ambiente de desenvolvimento (CMake, toolchain para Raspberry Pi Pico)

## Descrição

O projeto utiliza dois núcleos do Pico W para executar tarefas distintas:

- **Núcleo 0**:
  - Monitora o **botão no pino 5**.
  - Acende o **LED no pino 13** quando pressionado.

- **Núcleo 1**:
  - Monitora o **botão no pino 6**.
  - Acende o **LED no pino 11** quando pressionado.

As tarefas compartilham um recurso crítico simulado, controlado por **um mutex** e um **semáforo binário**. Um LED de debug no pino 25 pisca no início de cada tarefa para indicar que elas estão sendo executadas.

## Conceitos aplicados

- **FreeRTOS Tasks**: tarefas independentes que rodam em paralelo.
- **Multicore**: tarefas rodando em núcleos distintos com `multicore_launch_core1`.
- **Semaphore**: usado como sinalizador de acesso.
- **Mutex**: garante acesso exclusivo ao recurso crítico (uso simbólico no exemplo).
- **GPIO**: manipulação de pinos de entrada e saída no Pico W.
