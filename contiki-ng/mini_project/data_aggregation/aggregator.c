#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include <message.c>
#include <shared_conf.h>

static struct simple_udp_connection udp_conn;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
    if (datalen == sizeof(message)) {
        memcpy(&message, data, sizeof(message));
        print_message(message.node_id, message.sample, message.mean, message.variance);
    } else {
        // LOG_WARN("Received data of unexpected size\n");
    }
}

PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();

    // LOG_INFO("Starting UDP server\n");

    // Initialize UDP connection to listen on SERVER_PORT
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);

    PROCESS_END();
}
