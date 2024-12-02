#include "contiki.h"
#include <stdio.h>
#include "net/ipv6/simple-udp.h"
#include "sys/node-id.h"

#include <message.c>
#include <shared_conf.h>

#define NUM_NODES 3
#define TIMEOUT_THRESHOLD (CLOCK_SECOND * 5)

static struct etimer aggr_timer;
static struct simple_udp_connection udp_conn;
uip_ipaddr_t forward_ipaddr;
static message_t message_array[NUM_NODES];         // Array to store messages
static uint16_t node_map[NUM_NODES];               // Map to store node ids
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
double aggregate_data(double *variances, double *samples)
{
    double weighted_sum = 0.;
    double sum_weights = 0.;

    for (int i = 0; i < NUM_NODES; i++)
    {
        double weight = variances[i] == 0 ? 1 : 1. / variances[i];
        // double weight = 1. / variances[i];
        weighted_sum += weight * samples[i];
        sum_weights += weight;
    }

    return sum_weights != 0 ? weighted_sum / sum_weights : 0;
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
    // set ip address of the aggregator to forward to
    if (node_id == 2)
    {
        // send to node 1
        uip_ip6addr(&forward_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7401, 0x0001, 0x0101);
    }
    else if (node_id == 3)
    {
        // send to node 2
        uip_ip6addr(&forward_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7402, 0x0002, 0x0202);
    }
    // start timer when first message is received
    if (message_count == 0)
    {
        etimer_set(&aggr_timer, TIMEOUT_THRESHOLD);
    }
    message_agg_t recieved_agg_message;
    message_t message;
    if (datalen == sizeof(message) && node_id != 1)
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
                }
            }
        }
    }
    // if the message is an aggregated message
    else if (datalen == sizeof(recieved_agg_message))
    {
        memcpy(&recieved_agg_message, data, sizeof(recieved_agg_message));
        // forward the message to the next node unless it is the last node
        if (node_id != 1)
        {
            simple_udp_sendto(&udp_conn, &recieved_agg_message, sizeof(recieved_agg_message), &forward_ipaddr);
        }
        else
        {
            print_message_agg(recieved_agg_message);
        }
    }

    if (message_count == NUM_NODES || etimer_expired(&aggr_timer))
    {
        // aggregate the data
        double variances[NUM_NODES];
        double samples[NUM_NODES];
        construct_distribution_array(variances, samples);
        double aggregated_data = aggregate_data(variances, samples);

        // construct msg and print
        message_agg_t agg_message;
        agg_message.node_id = node_id;
        agg_message.sample = aggregated_data;
        agg_message.missed_samples = messages_missed;

        // print_message_agg(agg_message);

        // forward the message to the next node unless it is the last node
        if (node_id != 1)
        {
            simple_udp_sendto(&udp_conn, &agg_message, sizeof(agg_message), &forward_ipaddr);
        }

        // reset variables and timer
        message_count = 0;
        messages_missed = 0;
        for (int i = 0; i < NUM_NODES; i++)
        {
            nodes_recieved[i] = false;
        }
        etimer_reset(&aggr_timer);
    }
}

PROCESS_THREAD(udp_server_process, ev, data)
{
    PROCESS_BEGIN();

    // setup node map
    if(node_id == 2){
        // listen to 4, 5, 6
        node_map[0] = 4;
        node_map[1] = 5;
        node_map[2] = 6;
    }
    else if(node_id == 3){
        // listen to 7, 8, 9
        node_map[0] = 7;
        node_map[1] = 8;
        node_map[2] = 9;
    }
    
    simple_udp_register(&udp_conn, SERVER_PORT, NULL, CLIENT_PORT, udp_rx_callback);

    PROCESS_END();
}
