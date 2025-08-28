#include <stdio.h>
#include "pico/stdlib.h"
#include "init.h" 
#include <math.h>

int main() {
    // Esta função única inicializa tudo!
    initializeSystem(); 
    sleep_ms(1000);

    // Variáveis para os dados dos sensores
    float lux_value;
    
    // Buffer para formatar o texto para o display
    char buffer[40];

    // Variáveis para os dados do MPU6050
    int16_t ax, ay, az;
    char ax_str[30], ay_str[30], az_str[30];
    
    while (1) {
        // 1. Lê os dados brutos do sensor
        mpu6050_read_raw(&ax, &ay, &az);
          bh1750_read_lux(&lux_value);

        // 2. Calcula o ângulo de inclinação (vamos usar o Pitch - eixo X)
        // Usando a função mpu6050_get_pitch que criamos anteriormente
        float inclinacao_graus = mpu6050_get_pitch(ax, ay, az);

        // 3. Mapeia o ângulo de inclinação [-90, 90] para o ângulo do servo [0, 180]
        // O tipo 'long' da função map funciona bem com os floats aqui.
        uint angulo_servo = map(inclinacao_graus, -45, 45, 0, 180);

        // 4. Garante que o ângulo do servo permaneça dentro dos limites seguros (0-180)
        if (angulo_servo < 0) angulo_servo = 0;
        if (angulo_servo > 180) angulo_servo = 180;
        
        // 5. Comanda o servo para a nova posição
        servo_set_angle(angulo_servo);

        char text[50];
        
        sprintf(ax_str, "Eixo X: %.2f g", ax / ACCEL_SENSITIVITY);
        sprintf(ay_str, "Eixo Y: %.2f g", ay / ACCEL_SENSITIVITY);
        sprintf(az_str, "Eixo Z: %.2f g", az / ACCEL_SENSITIVITY);

        // Exibe dados do MPU6050
        updateTextLine(ax_str, 0, 0, 1, 128);
        updateTextLine(ay_str, 0, 10, 1, 128);
        updateTextLine(az_str, 0, 20, 1, 128);

        // Exibe dados do servo
        sprintf(text, "Angulo Servo: %d", angulo_servo);
        updateTextLine(text, 0, 40, 1, 128);


        // Exibe dados do BH1750
        sprintf(buffer, "Luz: %.1f lux", lux_value);
        updateTextLine(buffer, 0, 50, 1, 128);

        // Atualiza a tela com todas as informações
        ssd1306_show(&display);

        // Aguarda um pouco antes da próxima leitura
        sleep_ms(100); 
    }
    return 0; 
}