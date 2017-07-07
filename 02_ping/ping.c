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


//#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFF_SIZE 10

static uint8_t buff[BUFF_SIZE];


PROCESS(ping, "pinging");
AUTOSTART_PROCESSES(&ping);


/*--------------------------Unicast--------------------------------*/
static struct unicast_conn uc;
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
   rimeaddr_t receiver;
   char *packet=(char *)packetbuf_dataptr();

  if (rimeaddr_node_addr.u8[0]==1) {
    ;
    printf("Reply from %d.0 \n", from->u8[0]);
  }else {
  
      receiver.u8[0] = from->u8[0];
      receiver.u8[1] = 0;
      
      packetbuf_clear();
      packetbuf_copyfrom("Pong", 4);
      
      unicast_send(&uc, &receiver);
      printf("Pong \n");
  }
    
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};



/*---------------------------------------------------------------------------*/
PROCESS_THREAD(ping, ev, data)
{
  rimeaddr_t receiver;
  
  //PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
  cc2420_set_txpower(9);
   
  unicast_open(&uc, 146, &unicast_callbacks);
  
  
  
  if (rimeaddr_node_addr.u8[0]==1){ 
    
      receiver.u8[0] = 2;
      receiver.u8[1] = 0;
      
      packetbuf_clear();
      packetbuf_copyfrom("Ping", 4);
      
      unicast_send(&uc, &receiver);
      printf("Pinging %d.0 \n",receiver.u8[0]);
      
  }
	
    while (1)	{
		
		//Wait here for an event to happen
		PROCESS_WAIT_EVENT();
				

		// and loop
	}

  

  PROCESS_END();
}
