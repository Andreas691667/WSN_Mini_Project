#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "random.h"
#include "sys/log.h"
#include "sys/stimer.h" // Include stimer for timing

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765
#define SEND_INTERVAL (5 * CLOCK_SECOND)

static struct simple_udp_connection udp_conn;
static struct stimer send_timer; // Define an stimer

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
    static struct etimer periodic_timer;
    static struct {
        int sample;
        uint32_t timestamp;
    } payload; // Structure to hold data and timestamp

    PROCESS_BEGIN();

    // Register UDP connection
    simple_udp_register(&udp_conn, CLIENT_PORT, &server_ipaddr, SERVER_PORT, NULL);
    etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

    while (1) {       
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        // Set stimer and get timestamp
        stimer_reset(&send_timer);
        payload.sample = generate_uniform(min, max);
        payload.timestamp = stimer_expiration_time(&send_timer);

        LOG_INFO("Sending integer: %d with timestamp %lu\n", payload.sample, payload.timestamp);
        simple_udp_send(&udp_conn, &payload, sizeof(payload));

        etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}
