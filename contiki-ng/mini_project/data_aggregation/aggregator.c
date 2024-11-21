#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include <message.c>
#include <shared_conf.h>

#define NUM_NODES 3

static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;
static message_t message_array[NUM_NODES];                     // Array to store messages
static uint16_t node_map[NUM_NODES] = {2, 3, 4};               // Map to store node ids
static bool nodes_recieved[NUM_NODES] = {false, false, false}; // Array to store if nodes have sent a message
static uint16_t messages_missed = 0;
static int message_count = 0;

PROCESS(udp_server_process, "UDP server");
AUTOSTART_PROCESSES(&udp_server_process);

// Square function
double square(double x)
{
    return x * x;
}

// Aggregate the data by calculating the weighted sum of the means
double aggregate_data(double *variances, double *means)
{
    double weighted_sum = 0.;
    double sum_weights = 0.;

    for (int i = 0; i < NUM_NODES; i++)
    {
        double weight = 1. / variances[i];
        weighted_sum += weight * means[i];
        sum_weights += weight;
    }

    return weighted_sum / sum_weights;
}

// Construct the distribution array from the messages
void construct_distribution_array(double *variances, double *samples)
{
    for (int i = 0; i < NUM_NODES; i++)
    {
        variances[i] = message_array[i].variance;
        samples[i] = message_array[i].sample;
    }
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
    if (datalen == sizeof(message))
    {
        memcpy(&message, data, sizeof(message));

        // only store if there is no message for the node yet (i.e. 0)
        for (int i = 0; i < NUM_NODES; i++)
        {
            // overrides the old message if a new message is received from the same node that has already sent a message
            if (node_map[i] == message.node_id)
            {
                if (!nodes_recieved[i])
                {
                    message_array[i] = message;
                    nodes_recieved[i] = true;
                    message_count++;
                }
                else
                {
                    messages_missed++;
                    printf("Missed message from node %d\n", message.node_id);
                }
            }
        }
    }
    if (message_count == NUM_NODES)
    {

        // aggregate the data
        double variances[NUM_NODES];
        // double means[NUM_NODES];
        double samples[NUM_NODES];
        // construct_distribution_array(variances, means);
        construct_distribution_array(variances, samples);
        double aggregated_mean = aggregate_data(variances, samples);
        int aggregated_mean_int = aggregated_mean;
        double aggregated_mean_frac = (aggregated_mean - aggregated_mean_int) * 1000;

        printf("Aggregated data: %d.%03u, Missed: %d\n", aggregated_mean_int, (unsigned int)aggregated_mean_frac, messages_missed);

        message_count = 0;   // Reset message count
        messages_missed = 0; // Reset messages missed
        for (int i = 0; i < NUM_NODES; i++)
        {
            nodes_recieved[i] = false; // Reset nodes recieved
        }
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
