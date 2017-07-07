/*
 *
 *   Ping Command with Rime Primitives Unicas
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
void toggle();

PROCESS(blink, "blinking leds");
AUTOSTART_PROCESSES(&blink);


/*---------------------------------------------------------------------------*/
PROCESS_THREAD(blink, ev, data)
{
 static struct ctimer timerBlink;
  
  //PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
   
  leds_on(LEDS_RED);
  //leds_on(LEDS_GREEN);
  //leds_on(LEDS_BLUE);

    ctimer_set(&timerBlink,CLOCK_SECOND/32,toggle,NULL);
    
    while (1)	{
		
		
		//Wait here for an event to happen
		PROCESS_WAIT_EVENT();
				

		// and loop
	}

  

  PROCESS_END();
}

void toggle() {
   static struct ctimer timerBlink;
  leds_toggle(LEDS_RED);
  ctimer_set(&timerBlink,CLOCK_SECOND/32,toggle,NULL);
}
