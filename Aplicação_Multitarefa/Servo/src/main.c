#include <stdio.h>
#include "pico/stdlib.h"
#include "init.h"

int main() {
    // Inicializa a saída serial para podermos ver as mensagens com printf
    initializeSystem();
    sleep_ms(2000); // Delay para dar tempo de conectar o monitor serial

    // Inicializa o nosso servo
    updateTextLine("Servo SG90", 0, 0, 1, 128);
    ssd1306_show(&display);

    // Loop infinito para mover o servo
    while (1) {
         // --- Move o servo da posição mínima para a máxima ---
        updateTextLine("Movendo para minima", 0, 10, 1, 128);
        ssd1306_show(&display);
        printf("Movendo de 0 para 180 graus...\n");
        for (int angle = 0; angle <= 180; angle += 10) {
            servo_set_angle(angle);
            // Pequeno delay para controlar a velocidade do movimento
            // Um valor maior deixará o movimento mais lento
            sleep_ms(15); 
        }
        sleep_ms(500); // Pausa na posição máxima

        // --- Move o servo da posição máxima de volta para a mínima ---
        updateTextLine("Movendo para maxima", 0, 10, 1, 128);
        ssd1306_show(&display);
        printf("Movendo de 180 para 0 graus...\n");
        for (int angle = 180; angle >= 0; angle -= 10) {
            servo_set_angle(angle);
            sleep_ms(15);
        }
        sleep_ms(500); // Pausa na posição mínima
    }

    return 0; // Este código nunca será alcançado
}