#include "contiki.h"
#include <stdio.h>
#include "sht11-sensor.h" //for sensors

PROCESS(temp_read_process, "Temperature Process");
AUTOSTART_PROCESSES(&temp_read_process);

PROCESS_THREAD(temp_read_process, ev, data)
{
    SENSORS_ACTIVATE(sht11_sensor);
    val = sht11_sensor.value(SHT11_SENSOR_TEMP);

    SENSORS_DEACTIVATE(sht11_sensor);

    printf("%f", val);
}