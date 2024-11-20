#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include <message.c>
#include <shared_conf.h>

#define NUM_NODES 3
#define THRESHOLD 0.8


static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;
static message_t message_array[NUM_NODES]; // Array to store messages
static uint16_t node_map[NUM_NODES] = {2, 3, 4}; // Map to store node ids
static int message_index = 0;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

double square(double x) {
    return x * x;
}

static void udp_rx_callback(struct simple_udp_connection *c,
                            const uip_ipaddr_t *sender_addr,
                            uint16_t sender_port,
                            const uip_ipaddr_t *receiver_addr,
                            uint16_t receiver_port,
                            const uint8_t *data,
                            uint16_t datalen)
{
    message_t message;
    if (datalen == sizeof(message)) {
        memcpy(&message, data, sizeof(message));

        // only store if there is no message for the node yet (i.e. 0)
        for (int i = 0; i < NUM_NODES; i++) {
            // overrides the old message if a new message is received from the same node that has already sent a message
            if (node_map[i] == message.node_id) {
                message_array[i] = message;
                message_index++;
                print_message(message);
            }
        }
    } 
    if (message_index == NUM_NODES) {
        double sum_mean = 0., sum_variance = 0.;
        for (int i = 0; i < message_index; i++) {
            sum_mean += message_array[i].mean;
            sum_variance += message_array[i].variance;
        }
        double mean_mean = sum_mean / message_index; // mean of the distribution of the means
        double mean_variance = sum_variance / message_index; // mean of the distribution of the variances
        
        double variance_mean = 0.; // variance of the distribution of the means
        double variance_variance = 0.; // variance of the distribution of the variances
        for (int i = 0; i < message_index; i++) {
            variance_mean += (message_array[i].mean - mean_mean) * (message_array[i].mean - mean_mean);
            variance_variance += (message_array[i].variance - mean_variance) * (message_array[i].variance - mean_variance);
        }
        for (int i = 0; i < message_index; i++) {
            double z_mean = square(message_array[i].mean - mean_mean) / variance_mean;
            double z_variance = square(message_array[i].variance - mean_variance) / variance_variance; 
            
            printf("z_mean: %d, z_variance: %d\n", (int)(z_mean*100), (int)(z_variance*100));

            if (z_mean > THRESHOLD || z_variance > THRESHOLD) {
                printf("Node %d is an outlier\n", message_array[i].node_id);
            }
        }
        message_index = 0; // Reset message index
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
