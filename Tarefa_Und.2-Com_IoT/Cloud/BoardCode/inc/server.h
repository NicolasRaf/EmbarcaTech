#ifndef SERVER_H
#define SERVER_H

// Includes do C
#include <string.h>
#include "pico/stdlib.h"

// Includes do lwIP
#include "lwip/tcp.h"
#include "lwip/dns.h"
#include "lwip/sys.h"
#include "lwip/tcp.h"
#include "lwip/timeouts.h"  

// Includes do projeto
#include "ultrasonic.h"
#include "leds_buttons.h"
#include "joystick.h"

#define MAX_RETRIES      5
#define SEND_INTERVAL_MS 1000  // 1 segundo

#ifndef PROXY_PORT
    #define PROXY_PORT 1111
#endif

#ifndef PROXY_HOST
    #define PROXY_HOST ""
#endif

extern bool haveConnection;
extern int retries;

// Protótipos
err_t tcp_client_recv(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err);
err_t tcp_client_sent(void *arg, struct tcp_pcb *tpcb, u16_t len);
void  tcp_client_error(void *arg, err_t err);
err_t tcp_client_connected(void *arg, struct tcp_pcb *tpcb, err_t err);
void  create_tcp_connection(void);
static void do_tcp_write(const char *data, size_t len);
void  send_data_to_server(void);
void  close_tcp_connection(void);
void  resetConnection();

#endif // SERVER_H