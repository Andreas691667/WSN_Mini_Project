#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "random.h"
#include "sys/rtimer.h" // For timestamp

// logging
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765
#define SEND_INTERVAL (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;

PROCESS(udp_client, "UDP client");
AUTOSTART_PROCESSES(&udp_client);

int generate_uniform(int min, int max) {
    return min + (random_rand() % (max - min + 1));
}

PROCESS_THREAD(udp_client, ev, data)
{
    uip_ip6addr_t server_ipaddr;
    uip_ip6addr(&server_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7403, 0x0003, 0x0303);
    static int min = 0;
    static int max = 100;
    static struct etimer send_timer;
    static struct {
        int sample;
        uint32_t timestamp;
    } payload; // Struct to hold data and timestamp

    PROCESS_BEGIN();

    simple_udp_register(&udp_conn, CLIENT_PORT, &server_ipaddr, SERVER_PORT, NULL);
    etimer_set(&send_timer, random_rand() % SEND_INTERVAL);

    while (1) {       
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
        
        payload.sample = generate_uniform(min, max);
        payload.timestamp = RTIMER_NOW(); // Get current timestamp in ticks

        LOG_INFO("Sending integer: %d with timestamp %lu\n", payload.sample, payload.timestamp);
        simple_udp_send(&udp_conn, &payload, sizeof(payload));

        etimer_set(&send_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}
