#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwip/apps/http_client.h"
#include "leds_buttons.h"

#ifndef WIFI_SSID
    #define WIFI_SSID "SSID"
#endif

#ifndef WIFI_PASSWORD
    #define WIFI_PASSWORD "PASSWORD"
#endif

extern bool wifiConnected;

// Inicializa o Wi-Fi
int initWifi();

// Conecta ao Wi-Fi
int connectWifi();


#endif // WIFI_MANAGER_H
