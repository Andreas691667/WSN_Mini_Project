# Wireless Sensor Networks
## Assignment 2

### Approach
I base the signal strength of each channel on its *Received Signal Strength Indicator* (RSSI) value. 
To measure the RSSI value of each channel, I utilise the ```net/netstack.h``` library. To set the current channel, I use the function:
```c
NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, <channel_no>);
```
And to read the RSSI level, I use the funtion:
```c
radio_value_t current_rssi = 0;
NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &current_rssi);
```
According to the source code in ```radio.h```, the \texttt```RADIO_PARAM_RSSI``` parameter is the current RSSI value in dBm. Therefore, there is no need for conversion. 

Rather than basing comparison on a single value, I compute the average over $N$ values, and select the channel with highest average to be the best.
To allow for any changes to be present in the signal, I sleep for $t$ s between consecutive reads. The basic implementation is shown below:

```c
for (int i = CHANNEL_NUM_MIN; i <= CHANNEL_NUM_MAX; i++)
{
    NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, i);
    for (int j = 0; j < N; j++)
    {
        NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &current_rssi);
        total_rssi += current_rssi;
        wait_t(t);
    }

    current_rssi = total_rssi / READ_VALUES; 
    if (current_rssi > best_rssi)
    {
        best_rssi = current_rssi;
        best_channel = i;
    }
    total_rssi = 0;
}
```