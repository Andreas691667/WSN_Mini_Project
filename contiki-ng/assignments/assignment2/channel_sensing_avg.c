#include "contiki.h"
#include <stdio.h>
#include "dev/radio.h"
#include "net/netstack.h"

#define CHANNEL_NUM_MIN 11
#define CHANNEL_NUM_MAX 26
#define READ_VALUES 10

void wait_t(float time_s)
{
    // t is in seconds
    uint32_t t = time_s * CLOCK_SECOND;
    clock_wait(t);    
}

PROCESS(channel_sensing, "Channel Sensing");
AUTOSTART_PROCESSES(&channel_sensing);

PROCESS_THREAD(channel_sensing, ev, data)
{
    radio_value_t current_rssi, best_rssi = INT16_MIN;
    uint8_t best_channel = 0;
    long total_rssi = 0;

    int init_success = NETSTACK_RADIO.init();

    if (init_success != 1)
    {
        printf("Radio initialization failed\n");
        PROCESS_EXIT();
    }
    else
    {
        printf("Radio initialization successful\n");
    }

    PROCESS_BEGIN();

    for (int i = CHANNEL_NUM_MIN; i <= CHANNEL_NUM_MAX; i++)
    {
        // Set the radio channel
        NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, i);

        printf("Reading RSSI values on channel %d\n", i);

        // Read the RSSI value multiple times and calculate the average
        for (int j = 0; j < READ_VALUES; j++)
        {
            NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &current_rssi);
            total_rssi += current_rssi;
            printf("%d, ", current_rssi);
            wait_t(0.1);
        }

        current_rssi = total_rssi / READ_VALUES; 

        // print the channel number and the rssi value
        printf("Channel: %d, Average RSSI: %d\n", i, current_rssi);

        // if the rssi value is better than the best rssi value so far, update the best rssi value and the best channel number
        if (current_rssi > best_rssi)
        {
            best_rssi = current_rssi;
            best_channel = i;
        }

        total_rssi = 0;
    }

    // print the best channel number and the best rssi value
    printf("\n\n--------------------------------\n");
    printf("Best Channel: %d, Best RSSI: %d\n", best_channel, best_rssi);

    PROCESS_END();
}