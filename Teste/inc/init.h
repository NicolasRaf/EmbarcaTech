#ifndef INIT_H
#define INIT_H

#include <stdio.h> 
#include "wifi_manager.h"
#include "init.h"
#include "server.h"
#include "display.h"

void initializeSystem();
void initHttpServer();

void initGpio();
void initI2C();
void clearScreen();

#endif // INIT_H
