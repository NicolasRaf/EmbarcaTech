#ifndef SERVER_H
#define SERVER_H

// Includes do C
#include <string.h>
#include "pico/stdlib.h"

// Includes do lwIP
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/timeouts.h"

// Includes do projeto
#include "ultrasonic.h"
#include "leds_buttons.h"
#include "joystick.h"
#include "wifi_manager.h"

#define MAX_RETRIES 5
#define SEND_INTERVAL_MS 1000

#ifndef PROXY_PORT
    #define PROXY_PORT 11111
#endif

#ifndef PROXY_HOST
    #define PROXY_HOST ""
#endif

static bool haveConnection;
static int retries;
static volatile bool can_send;

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void send_data_to_server();
void create_tcp_connection();
void close_tcp_connection();

#endif // SERVER_H