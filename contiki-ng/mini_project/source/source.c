#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "net/netstack.h"
#include "random.h"

// logging
#include "sys/log.h"
#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765
#define SEND_INTERVAL (5 * CLOCK_SECOND)  // Send every 5 seconds
#define M_PI 3.1415926

static struct simple_udp_connection udp_conn;

PROCESS(udp_client, "UDP client");
AUTOSTART_PROCESSES(&udp_client);

int generate_gaussian(int mean, int std_dev){
    // float u1 = (float)random_rand() / RANDOM_RAND_MAX;
    // float u2 = (float)random_rand() / RANDOM_RAND_MAX;
    // float z0 = sqrt(-2.0 * log(u1)) * cos(2 * M_PI * u2);
    // return (int)(z0 * std_dev + mean + 0.5); // Round to the nearest integer
    return mean+std_dev;
}

PROCESS_THREAD(udp_client, ev, data)
{
    uip_ip6addr_t server_ipaddr;
    uip_ip6addr(&server_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202);

    static struct etimer send_timer;

    PROCESS_BEGIN();

    // Register UDP connection
    simple_udp_register(&udp_conn, CLIENT_PORT, &server_ipaddr, SERVER_PORT, NULL);

    // Set the timer for periodic message sending
    etimer_set(&send_timer, random_rand() % SEND_INTERVAL);

    while (1) {       
        // Wait for the timer to expire
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&send_timer));
        // Generate Gaussian-distributed integer
        int mean = 50;       // Example mean
        int std_dev = 15;    // Example standard deviation
        int gaussian_int = generate_gaussian(mean, std_dev);

        printf("Sending Gaussian integer: %d to:", gaussian_int);
        LOG_INFO_6ADDR(&server_ipaddr);
        printf("\n");
        simple_udp_send(&udp_conn, &gaussian_int, sizeof(gaussian_int));

           /* Add some jitter */
    etimer_set(&send_timer, SEND_INTERVAL
      - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}