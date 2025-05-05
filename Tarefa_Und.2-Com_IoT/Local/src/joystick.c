#include "joystick.h"

// Variáveis globais definidas aqui
char directionWindRose[3] = "C";
int x_value = 0;
int y_value = 0;

void calculate_direction() {
    if (x_value > 3000 && y_value > 3000) strcpy(directionWindRose, "NE");
    else if (x_value > 3000 && y_value < 1000) strcpy(directionWindRose, "SE");
    else if (x_value < 1000 && y_value > 3000) strcpy(directionWindRose, "NW");
    else if (x_value < 1000 && y_value < 1000) strcpy(directionWindRose, "SW");
    else if (x_value > 3000) strcpy(directionWindRose, "E");
    else if (x_value < 1000) strcpy(directionWindRose, "W");
    else if (y_value > 3000) strcpy(directionWindRose, "N");
    else if (y_value < 1000) strcpy(directionWindRose, "S");
    else strcpy(directionWindRose, "C");
}

void read_joystick() {
    adc_select_input(0);
    x_value = adc_read();
    adc_select_input(1);
    y_value = adc_read();
    calculate_direction();
}
