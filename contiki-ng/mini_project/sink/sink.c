#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"

// for logging
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765

static struct simple_udp_connection udp_conn;

PROCESS(udp_server, "UDP server");
AUTOSTART_PROCESSES(&udp_server);

// UDP message reception callback function
static void udp_rx_callback(struct simple_udp_connection *conn,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
    // square the received integer
    int received_number = *((int *) data);
    LOG_INFO("Received data: %d from ", received_number);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");
}

// static void set_server_ip_address(void)
// {
// 	// construct the server's IP address
// 	uip_ip6addr_t ipaddr;
//   uip_ip6addr(&ipaddr, 0xfe80, 0, 0, 0, 0x0202, 0, 0x0002, 0x0002);	

// 	// set the server's IP address
//   printf("Server IP address set to ");
// }

PROCESS_THREAD(udp_server, ev, data)
{
    PROCESS_BEGIN();

    // set_server_ip_address();

    // Register UDP connection and callback function
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);


    PROCESS_END();
}