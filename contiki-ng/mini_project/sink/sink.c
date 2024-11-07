#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "sys/rtimer.h" // For timestamp

// logging
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765

static struct simple_udp_connection udp_conn;

PROCESS(udp_server, "UDP server");
AUTOSTART_PROCESSES(&udp_server);

static void udp_rx_callback(struct simple_udp_connection *conn,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
    struct {
        int sample;
        uint32_t timestamp;
    } payload; // Struct to match the client payload

    if (datalen == sizeof(payload)) {
        memcpy(&payload, data, sizeof(payload));

        uint32_t current_time = RTIMER_NOW();
        uint32_t latency = current_time - payload.timestamp;

        LOG_INFO("Received data: %d from ", payload.sample);
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_(" with latency: %lu ticks\n", latency);
    }
}

PROCESS_THREAD(udp_server, ev, data)
{
    PROCESS_BEGIN();
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);
    PROCESS_END();
}
