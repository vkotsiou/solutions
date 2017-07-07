/*
 *
 *   Using the on-board LEDs
 *  
 */

#include "project-conf.h"
#include "contiki.h"
#include "dev/leds.h"
#include "net/rime.h"
#include "net/netstack.h"
#include "sys/ctimer.h"

#include "lib/random.h"


#include "net/mac/contikimac.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


PROCESS(blink, "blinking leds");
AUTOSTART_PROCESSES(&blink);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink, ev, data)
{
  static struct etimer et;
  
  //PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
   
  leds_on(LEDS_RED);
  //leds_on(LEDS_GREEN);
  //leds_on(LEDS_BLUE);

	
    while (1)	{
		etimer_set(&et, CLOCK_SECOND/16); 	
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); // Wait for the etimer expiration
		
		leds_toggle(LEDS_RED);
		//Wait here for an event to happen
		//PROCESS_WAIT_EVENT();
				

		// and loop
	}

  

  PROCESS_END();
}
