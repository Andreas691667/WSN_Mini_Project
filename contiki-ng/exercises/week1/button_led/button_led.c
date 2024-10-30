#include "contiki.h"

#include <stdio.h> /* For printf() */

#include "dev/button-sensor.h"
#include "leds.h"

/*---------------------------------------------------------------------------*/
PROCESS(process, "process");
AUTOSTART_PROCESSES(&process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(process, ev, data)
{
  static struct etimer timer;
  static int colors[3] = {LEDS_RED, LEDS_YELLOW, LEDS_GREEN};
  static int color = 0;

  PROCESS_BEGIN();

  /* Setup a periodic timer that expires after 1 second. */
  etimer_set(&timer, CLOCK_SECOND * 1);

  SENSORS_ACTIVATE(button_sensor);

  while(1) {

    PROCESS_WAIT_EVENT();

    /* Check if the button is pressed */
    if (ev == sensors_event && data == &button_sensor) {
      leds_off(LEDS_ALL);
      leds_on(colors[color]);
      if (color == 2) {
        color = 0;
      }
      else {
        color += 1;
      }

    }

    if (etimer_expired(&timer)) {
      etimer_reset(&timer);
    }
  }


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
