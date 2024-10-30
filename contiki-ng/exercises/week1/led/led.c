#include "contiki.h"

#include <stdio.h> /* For printf() */

#include "dev/light-sensor.h"
#include "sht11-sensor.h"

/*---------------------------------------------------------------------------*/
PROCESS(process, "process");
AUTOSTART_PROCESSES(&process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process, ev, data)
{
  static struct etimer timer;
  // static double light;
  static float temp;
  static float hum;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 1 second. */
  etimer_set(&timer, CLOCK_SECOND * 1);

  while(1) {
    float real_hum;
    float real_temp;

    SENSORS_ACTIVATE(light_sensor);
    SENSORS_ACTIVATE(sht11_sensor);

    temp = sht11_sensor.value(SHT11_SENSOR_TEMP);
    hum = sht11_sensor.value(SHT11_SENSOR_HUMIDITY);
    // light = light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC);

    real_temp =  -39.6 + 0.01*temp;
    real_hum = -2.0468 + 0.0367 * hum + -1.5955e-6 * (hum*hum);

    int A;
    float frac;
    A=real_temp;
    frac = (real_temp-A)*100;
    printf("Temperature = %d.%02u \n", A,(unsigned int)frac);

    A=real_hum;
    frac = (real_hum-A)*100;
    printf("Humidity = %d.%02u \n\n", A,(unsigned int)frac);

    // printf("Temperature = (%d)\n", temp);
    // printf("Humidity = (%f)", real_hum);
    /* Wait for the periodic timer to expire and then restart the timer. */
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&timer));
    etimer_reset(&timer);

    SENSORS_DEACTIVATE(light_sensor);
    SENSORS_DEACTIVATE(sht11_sensor);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
