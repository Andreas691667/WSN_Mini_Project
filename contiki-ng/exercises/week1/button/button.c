#include "contiki.h"

#include <stdio.h> /* For printf() */

#include "dev/button-sensor.h"
/*---------------------------------------------------------------------------*/
PROCESS(process, "process");
AUTOSTART_PROCESSES(&process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process, ev, data)
{
  static struct etimer timer;
  static int counter;
  static int direction = 1;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 1 second. */
  etimer_set(&timer, CLOCK_SECOND * 1);

  SENSORS_ACTIVATE(button_sensor);

  while(1) {

    PROCESS_WAIT_EVENT();

    /* Check if the button is pressed */
    if (ev == sensors_event && data == &button_sensor) {
      direction = -direction; // Reverse the direction
    }

    if (etimer_expired(&timer)) {
      counter += direction;
      printf("Counter = (%d)\n", counter);
      etimer_reset(&timer);

    }
  }


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
