#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include <message.c>
#include <shared_conf.h>
#define MAX_MESSAGES 20

static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;
static message_t message_array[MAX_MESSAGES]; // Array to store messages
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
    if (datalen == sizeof(message)) {
        memcpy(&message, data, sizeof(message));
        message_array[message_index] = message;
        message_index++;
        print_message(message);
    } 
    if (message_index = 3) {
        double sum_mean = 0.;
        for (int i = 0; i < message_index; i++) {
            sum_mean += message_array[i].mean;
        }
        double sum_variance = 0.;
        for (int i = 0; i < message_index; i++) {
            sum_variance += message_array[i].variance;
        }
        double mean_mean = sum_mean / message_index;
        double variance_mean = sum_variance / message_index;
        double std_mean = 0.;
        double std_variance = 0.;
        for (int i = 0; i < message_index; i++) {
            std_mean += (message_array[i].mean - mean_mean) * (message_array[i].mean - mean_mean);
            std_variance += (message_array[i].variance - variance_mean) * (message_array[i].variance - variance_mean);
        }
        for (int i = 0; i < message_index; i++) {
            if (square(message_array[i].mean - mean_mean) / std_mean > 1 || square(message_array[i].variance - variance_mean) / std_variance > 1) {
                printf("Node %d is an outlier\n", message_array[i].node_id);
            }
        }
    }
} 

PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();

    // LOG_INFO("Starting UDP server\n");

    // Initialize UDP connection to listen on SERVER_PORT
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, SERVER_PORT, udp_rx_callback);

    PROCESS_END();
}
