#include "contiki.h"
#include <stdio.h>
#include <stdlib.h>
#include "net/ipv6/simple-udp.h"
#include "random.h"
#include "sys/clock.h"
#include "sys/node-id.h"
#include <message.c>
#include <shared_conf.h>

#define SEND_INTERVAL (5 * CLOCK_SECOND)
#define SAMPLE_MIN 0
#define SAMPLE_MAX 100
#define DELTA 10
#define WINDOW_SIZE 100.0

static struct simple_udp_connection udp_conn;
uip_ipaddr_t server_ipaddr;

PROCESS(udp_client_process, "UDP client");
AUTOSTART_PROCESSES(&udp_client_process);

int generate_uniform(int min, int max)
{
	return min + (random_rand() % (max - min + 1));
}

// Initialize the minimum and maximum values for the uniform distribution
void initialize_distribution_param(int *min, int *max)
{
	if (*min == -1)
	{
		*min = random_rand() % ((SAMPLE_MIN + DELTA) - (SAMPLE_MIN - DELTA) + 1) + (SAMPLE_MIN - DELTA);
	}
	if (*max == -1)
	{
		*max = random_rand() % ((SAMPLE_MAX + DELTA) - (SAMPLE_MAX - DELTA) + 1) + (SAMPLE_MAX - DELTA);
	}
}

// update the distribution parameters every 100 samples
void update_distribution(int sample, int *sample_num, double *running_mean, double *running_difference, double *mean, double *variance)
{
	*running_mean += (double)sample / WINDOW_SIZE;
	// using the last mean to calculate the running mean difference
	// potential problem: the mean is 0 in the first 100 samples
	*running_difference += (abs(*mean - (double)sample) * abs(*mean - (double)sample)) / WINDOW_SIZE;

	if (*sample_num == (int)WINDOW_SIZE)
	{
		*sample_num = 0;
		*mean = *running_mean;
		*variance = *running_difference;
		*running_mean = 0.;
		*running_difference = 0.;
	}
}

PROCESS_THREAD(udp_client_process, ev, data)
{
	static struct etimer periodic_timer;

	static int min = -1, max = -1; // initialize to -1 signifying they need to be set
	static int sample_num = 0;
	static double mean = 0.;
	static double running_mean = 0.;
	static double variance = 0.;
	static double running_difference = 0.;

	PROCESS_BEGIN();

	// to avoid generating identical numbers
	random_init(node_id);

	// Register UDP connection
	simple_udp_register(&udp_conn, CLIENT_PORT, NULL, SERVER_PORT, NULL);

	// Set hardcoded server IP address
	uip_ip6addr(&server_ipaddr, 0xfe80, 0, 0, 0, 0x0212, 0x7401, 0x0001, 0x0101);

	// Set up random sampling range
	initialize_distribution_param(&min, &max);

	// Schedule the first transmission
	etimer_set(&periodic_timer, random_rand() % SEND_INTERVAL);

	while (1)
	{
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));

		message_t message;
		// Generate a sample and prepare the message
		message.sample = generate_uniform(min, max);

		// Update the distribution parameters
		sample_num++;
		update_distribution(message.sample, &sample_num, &running_mean, &running_difference, &mean, &variance);

		message.node_id = node_id;
		message.mean = mean;
		message.variance = variance;

		print_message(message.node_id, message.sample, message.mean, message.variance);

		// Send the UDP packet to the server
		simple_udp_sendto(&udp_conn, &message, sizeof(message), &server_ipaddr);

		// Reschedule the next transmission with some jitter
		etimer_set(&periodic_timer, SEND_INTERVAL - CLOCK_SECOND + (random_rand() % (2 * CLOCK_SECOND)));
	}

	PROCESS_END();
}
