#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "random.h"
#include "sys/log.h"
#include "sys/clock.h" // Include clock for finer timing
#include "sys/node-id.h"
#include "net/netstack.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO

#define SERVER_PORT 5678
#define CLIENT_PORT 8765
#define SEND_INTERVAL (5 * CLOCK_SECOND)
#define SAMPLE_MIN 0
#define SAMPLE_MAX 100
#define DELTA 10

static struct simple_udp_connection udp_conn;

static int min, max = -1;

static int window_size = 100;
static int sample_num = 0;
static double mean = 0.;
static double running_mean = 0.;
static double variance = 0.;
static double running_squared_mean = 0.;

// typedef struct {
//     int mote_id;
//     int sample;
//     double variance;
//     double mean;
// } message_t;

PROCESS(udp_client, "UDP client");
AUTOSTART_PROCESSES(&udp_client);

int generate_uniform(int min, int max) {
    return min + (random_rand() % (max - min + 1));
}

// Initialize the minimum and maximum values for the uniform distribution
void initialize_distribution_param() {
    if (min == -1) {
        min = random_rand() % ((SAMPLE_MIN+DELTA) - (SAMPLE_MIN-DELTA) + 1) + (SAMPLE_MIN-DELTA);
    }
    if (max == -1) {
        max = random_rand() % ((SAMPLE_MAX+DELTA) - (SAMPLE_MAX-DELTA) + 1) + (SAMPLE_MAX-DELTA);
    }
}

void update_distribution(int sample, int sample_num) {
    running_mean += sample / window_size;
    running_squared_mean += (sample * sample) / window_size;
    
    if (sample_num == 100) {
        sample_num = 0;
        mean = running_mean;
        variance = running_squared_mean - (mean * mean);
        running_mean = 0.;
        running_squared_mean = 0.;
    }
}


PROCESS_THREAD(udp_client, ev, data)
{
    static struct etimer periodic_timer;
    // static message_t message;
    uip_ip6addr_t server_ipaddr;
    uip_ip6addr(&server_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7403, 0x0003, 0x0303);

    // static struct {
    //     // uint16_t mote_id;
    //     int sample;
    //     // double variance;
    //     // double mean;
    // } message;

    static struct {
        int sample;
    } message;

    PROCESS_BEGIN();

    NETSTACK_MAC.on();
    NETSTACK_RADIO.on();

    // Register UDP connection
    simple_udp_register(&udp_conn, CLIENT_PORT, NULL, SERVER_PORT, NULL);
    etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

    initialize_distribution_param();

    while (1) {       
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

        message.sample = generate_uniform(min, max);
        
        sample_num++;
        // update_distribution(message.sample, sample_num);

        // message.mote_id = node_id;
        // message.mean = mean;
        // message.variance = variance;

        // printf("mote_id: %d, sample: %d, mean: %f, variance: %f\n", message.mote_id, message.sample, message.mean, message.variance);
        // LOG_INFO("mote_id: %d, sample: %d\n", message.mote_id, message.sample);
        LOG_INFO("sample: %d\n", message.sample);
        // simple_udp_send(&udp_conn, &message, sizeof(message));
        simple_udp_sendto(&udp_conn, &message, sizeof(message), &server_ipaddr);

        // LOG_INFO_6ADDR(&server_ipaddr);

        etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
    }

    PROCESS_END();
}
