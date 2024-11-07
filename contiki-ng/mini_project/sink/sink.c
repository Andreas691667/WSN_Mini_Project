#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "sys/clock.h" // Include clock for timing
#include "sys/log.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765

static struct simple_udp_connection udp_conn;

PROCESS(udp_server, "UDP server");
AUTOSTART_PROCESSES(&udp_server);

// UDP message reception callback
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
        clock_time_t timestamp;
    } payload;

    if (datalen == sizeof(payload)) {
        memcpy(&payload, data, sizeof(payload));

        // Calculate latency
        clock_time_t current_time = clock_time();
        clock_time_t latency = current_time - payload.timestamp;

        LOG_INFO("Received data: %d from ", payload.sample);
        LOG_INFO_6ADDR(sender_addr);
        LOG_INFO_(" with latency: %lu ticks (%lu ms)\n", (unsigned long)latency, (unsigned long)(latency * 1000 / CLOCK_SECOND));
    }
}

PROCESS_THREAD(udp_server, ev, data)
{
    PROCESS_BEGIN();

    // Register UDP connection and callback
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);

    PROCESS_END();
}
