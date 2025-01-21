#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "music.h"

#define WIFI_SSID "HUAWEI-2.4G-S7mw"
#define WIFI_PASSWORD "labirasifpi"

// Prototipos das funções
void failureAction(char *debug);
void succeedAction(char *debug);
int init_wifi();
int connect_wifi(const char *ssid, const char *password);
void print_ip_address();

#endif // WIFI_MANAGER_H
