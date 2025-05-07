#ifndef SERVER_H
#define SERVER_H

// Includes do C
#include <string.h>
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "pico/stdlib.h"

// Includes do projeto
#include "ultrasonic.h"
#include "leds_buttons.h"
#include "joystick.h"

#define MAX_RETRIES 5 

#ifndef PROXY_PORT
    #define PROXY_PORT 1111
#endif

#ifndef PROXY_HOST
    #define PROXY_HOST ""
#endif

extern bool haveConnection;
extern int retries;

err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
void send_data_to_server();
void create_tcp_connection();
void close_tcp_connection();
void resetConnection();

#endif // SERVER_H