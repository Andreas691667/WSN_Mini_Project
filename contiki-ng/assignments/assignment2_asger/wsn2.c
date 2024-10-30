#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <stdint.h>
#include "dev/radio.h"
#include "net/netstack.h"

#define CHANNEL_START 11
#define CHANNEL_END 26
#define RSSI_SAMPLES 10

// Function to measure RSSI on a specific channel
int16_t measure_rssi(uint8_t channel) {
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
    int32_t rssi_sum = 0;

    for (int i = 0; i < RSSI_SAMPLES; i++) {
        int16_t rssi;
        NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &rssi); // Get RSSI in dBm directly
        rssi_sum += rssi;
        clock_delay(1000); // 100 ms delay between samples (adjust as needed)
    }

    return rssi_sum / RSSI_SAMPLES; // Average RSSI for stability
}

/*---------------------------------------------------------------------------*/
PROCESS(wsn2, "wsn2 assginment");
AUTOSTART_PROCESSES(&wsn2);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(wsn2, ev, data)
{

  PROCESS_BEGIN();
    int16_t min_rssi = 127; // Max RSSI for initialization
    uint8_t best_channel = CHANNEL_START;

  while(1) {
    for (uint8_t channel = CHANNEL_START; channel <= CHANNEL_END; channel++) {
        int16_t rssi = measure_rssi(channel);
        if (rssi < min_rssi) {
            min_rssi = rssi;
            best_channel = channel;
        }
    }

    printf("Best Channel: %d with RSSI: %d dBm\n", best_channel, min_rssi);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/