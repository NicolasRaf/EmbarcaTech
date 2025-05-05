#ifndef SERVER_H
#define SERVER_H

#include "pico/cyw43_arch.h"
#include "leds_buttons.h"
#include "hardware/adc.h"
#include "html_response.h"
#include "joystick.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>

#include "pico/stdlib.h"
#include "lwip/tcp.h"
#include "lwip/pbuf.h"

#define PORT 80

void start_server();

#endif
