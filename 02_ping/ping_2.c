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
#define NO_OFF_PACKETS 1000



static clock_time_t start_time,end_time,time_interval;
void send_packet();
void show_stats();
static unsigned int packets=0,received_packets=0 ,min=4000,max=0,avg=0,sum=0;

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
    end_time=clock_time();
    time_interval= (end_time-start_time)*1000/CLOCK_SECOND;
    printf("Reply from %d.0 time=%ld ms \n", from->u8[0],time_interval);
    
    ++received_packets;
    sum+=time_interval;
    if (time_interval<min) {
       min=time_interval;
    }
    if (time_interval>max) {
       max=time_interval;
    }
    
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
  cc2420_set_txpower(5);
   
  unicast_open(&uc, 146, &unicast_callbacks);
  
  
  
  if (rimeaddr_node_addr.u8[0]==1){ 
    
      send_packet();
      
  }
	
    while (1)	{
		
		//Wait here for an event to happen
		PROCESS_WAIT_EVENT();
				

		// and loop
	}

  

  PROCESS_END();
}

void send_packet() {
 static struct ctimer timerPing,timerStats;
 rimeaddr_t receiver;
  if (packets<NO_OFF_PACKETS) {
      receiver.u8[0] = 2;
      receiver.u8[1] = 0;
      
      packetbuf_clear();
      packetbuf_copyfrom("Ping", 4);
      packets++;
      start_time=clock_time();
      
      unicast_send(&uc, &receiver);
      
     
      printf("Pinging %d.0 %d packet  \n",receiver.u8[0],packets);
      
      
	ctimer_set(&timerPing, CLOCK_SECOND /64, send_packet, NULL);
    
  }else {

     ctimer_set(&timerStats,CLOCK_SECOND*3,show_stats,NULL);
  }
	
	
      
}

void show_stats(){
  avg=sum/received_packets;
  printf("Ping Statistics \n");
  printf("  Packets: Sent = %d Received = %d Lost = %d \n", packets,received_packets,packets-received_packets);
  printf("Approximate round trip times in milli-seconds: \n");
  printf("  Minimum = %d ms, Maximum = %d ms, Average = %d ms \n",min,max,avg);
}