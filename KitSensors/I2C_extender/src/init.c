// init.c
#include "init.h"

// Inicializa o I2C1 para o display
void initI2C_Display() {
    i2c_init(i2c1, 400 * 1000);
    gpio_set_function(I2C_SDA_DISPLAY, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_DISPLAY, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_DISPLAY);
    gpio_pull_up(I2C_SCL_DISPLAY);
}

// Inicializa o I2C0 para os sensores
void initI2C_Sensors() {
    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(I2C_SDA_SENSORS, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_SENSORS, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_SENSORS);
    gpio_pull_up(I2C_SCL_SENSORS);
}

// Função principal de inicialização do sistema
void initializeSystem(void) {
    stdio_init_all();
    sleep_ms(1000);

    // Configura os dois barramentos I2C
    printf("Configurando I2C do OLED (I2C1)...\n");
    initI2C_Display();
    
    printf("Configurando I2C dos Sensores (I2C0)...\n");
    initI2C_Sensors();

    // Inicializa o display
    printf("Iniciando SSD1306...\n");
    if (initializeDisplay() != 0) {
        printf("Erro ao inicializar o SSD1306\n");
    }

    printf("Configurando o MPU-6050...\n");
    mpu6050_init(); // usa i2c0 internamente

    printf("Configurando o BH1750...\n");
    bh1750_init(); // usa i2c0
    
    printf("Configurando o servo...\n");
    servo_init(); 

    printf("Tela limpa.\n");
    clearScreen();
}