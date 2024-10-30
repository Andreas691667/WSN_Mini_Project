#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "net/routing/routing.h"
#include "net/netstack.h"

// for logging
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define WITH_SERVER_REPLY true
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
    int squared_number = received_number * received_number;
    LOG_INFO("Received number: %d from ", received_number);
    LOG_INFO_6ADDR(sender_addr);
    LOG_INFO_("\n");
    LOG_INFO("Number squared: %d\n", squared_number);

    // LOG_INFO("Received message '%.*s' from ", datalen, (char *) data);
    // LOG_INFO_6ADDR(sender_addr);
    // LOG_INFO_("\n");
    // if (WITH_SERVER_REPLY)
    // {
    //     LOG_INFO("Sending response.\n");
    //     simple_udp_sendto(&udp_conn, data, datalen, sender_addr);
    // }
}

PROCESS_THREAD(udp_server, ev, data)
{
    PROCESS_BEGIN();

    // Set the node as a routing root
    NETSTACK_ROUTING.root_start();

    // Register UDP connection and callback function
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);

    PROCESS_END();
}