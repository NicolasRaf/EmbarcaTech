#include "setup.h"

void initializeSystem() {
    stdio_init_all();

    initJoystick();
    initWifi();
    initGpioButtonLeds();
    initHcsr04();
}
