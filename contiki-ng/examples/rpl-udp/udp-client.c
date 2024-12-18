#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "random.h"
#include "sys/log.h"
#include "sys/clock.h"
#include "sys/node-id.h"

#define LOG_MODULE "Client"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765
#define SEND_INTERVAL (5 * CLOCK_SECOND)
#define SAMPLE_MIN 0
#define SAMPLE_MAX 100
#define DELTA 10

static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;

PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

int generate_uniform(int min, int max) {
    return min + (random_rand() % (max - min + 1));
}

// Initialize the minimum and maximum values for the uniform distribution
void initialize_distribution_param(int *min, int *max) {
    if (*min == -1) {
        *min = random_rand() % ((SAMPLE_MIN + DELTA) - (SAMPLE_MIN - DELTA) + 1) + (SAMPLE_MIN - DELTA);
    }
    if (*max == -1) {
        *max = random_rand() % ((SAMPLE_MAX + DELTA) - (SAMPLE_MAX - DELTA) + 1) + (SAMPLE_MAX - DELTA);
    }
}

PROCESS_THREAD(udp_client_process, ev, data)
{
    static struct etimer periodic_timer;
    static struct {
        uint16_t node_id;
        int sample;
    } message;

    static int min = -1, max = -1;

    PROCESS_BEGIN();

    // Register UDP connection
    simple_udp_register(&udp_conn, CLIENT_PORT, NULL, SERVER_PORT, NULL);

    // Set hardcoded server IP address
    uip_ip6addr(&server_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202);

    // Set up random sampling range
    initialize_distribution_param(&min, &max);

    // Schedule the first transmission
    etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

    while (1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        // Generate a sample and prepare the message
        message.sample = generate_uniform(min, max);
        message.node_id = node_id;
        LOG_INFO("node_id: %d, Sending sample: %d to ", message.node_id, message.sample);
        LOG_INFO_6ADDR(&server_ipaddr);
        LOG_INFO_("\n");

        // Send the UDP packet to the server
        simple_udp_sendto(&udp_conn, &message, sizeof(message), &server_ipaddr);

        // Reschedule the next transmission with some jitter
        etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}
