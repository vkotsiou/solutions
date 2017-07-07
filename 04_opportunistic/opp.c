/*
 *
 *   Opportunistic Routing with Rime Primitives broadcast and unicast
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
#define GRAND_DELAY 45
#define CBR_RATE 10


static uint8_t My_Rank =100,seq=0;
void send_gradient(),generate_packet();

static uint8_t buff[BUFF_SIZE],fatherID=-1;

#define MAX_TABLE_REC 20

static uint8_t num_recs=0;
static uint16_t total_trans=0;

struct neighbor_rec {
  struct neighbor_rec *next;
  rimeaddr_t  addr ;
  uint8_t rank;
};


MEMB(neighbor_memb, struct neighbor_rec, MAX_TABLE_REC);

LIST(neighbor_list);

void add_neighbor(rimeaddr_t,uint8_t);
void remove_neighbor(rimeaddr_t),remove_all();
uint8_t find_father();

PROCESS(opp, "routing test");
AUTOSTART_PROCESSES(&opp);

/*---------------------Broadcast----------------------------------------------------*/
// Receiver 
static void broadcast_recv(struct broadcast_conn *c, const rimeaddr_t *from)
{
  static struct ctimer timerNd;
  char *packet=(char *)packetbuf_dataptr();
  uint8_t rcv_rank=(uint8_t)packet[0];
  rimeaddr_t n;
 
    printf("Gradient Received from %d.%d:  with Rank %d ^ %d \n",from->u8[0], from->u8[1], rcv_rank,My_Rank);
    if (rcv_rank+1<My_Rank) {
      
      static unsigned int counter = 0;
      
      My_Rank=rcv_rank+1;
      fatherID=from->u8[0];
      
      counter = random_rand();
      counter %= 10;
      counter += 1;
      
      ctimer_set(&timerNd, CLOCK_SECOND *counter, send_gradient, NULL);
      printf("My New Rank is %d Father %d \n",My_Rank,fatherID);
      
      n.u8[0]=from->u8[0];
      n.u8[1]=from->u8[1];
      remove_all();
      add_neighbor(n,My_Rank);
      
    }else if (rcv_rank+1==My_Rank) {
      n.u8[0]=from->u8[0];
      n.u8[1]=from->u8[1];
      add_neighbor(n,My_Rank);
      
    }
  
}
static const struct broadcast_callbacks broadcast_call = {broadcast_recv};
static struct broadcast_conn broadcast;

/*--------------------------Unicast--------------------------------*/
static struct unicast_conn uc;
static void
recv_uc(struct unicast_conn *c, const rimeaddr_t *from)
{
   char *packet=(char *)packetbuf_dataptr();

  if (rimeaddr_node_addr.u8[0]==1) {
    printf("Sink from %d -- Seqno %d  from %d  \n",packet[0],packet[1],from->u8[0]);
  }else { // forward
      rimeaddr_t receiver;

      buff[0]=rimeaddr_node_addr.u8[0];
  
      receiver.u8[0] = fatherID;
      receiver.u8[1] = 0;
      
      packetbuf_clear();
      packetbuf_copyfrom(packet, BUFF_SIZE);
      
      printf("Forwarding \n");
      unicast_send(&uc, &receiver);
    
  }
}
static const struct unicast_callbacks unicast_callbacks = {recv_uc};



/*---------------------------------------------------------------------------*/
PROCESS_THREAD(opp, ev, data)
{
  static struct etimer et;
  static struct ctimer timerSnk,timerEmt;
   
  //PROCESS_EXITHANDLER(broadcast_close(&broadcast);)

  PROCESS_BEGIN();
  cc2420_set_txpower(9);
   
  broadcast_open(&broadcast, 129, &broadcast_call);
  unicast_open(&uc, 146, &unicast_callbacks);
  
  
  
  if (rimeaddr_node_addr.u8[0]==1){ // Sink 
      My_Rank=0;
      ctimer_set(&timerSnk, CLOCK_SECOND *2, send_gradient, NULL);
  }else { // Nodes
      ctimer_set(&timerEmt, CLOCK_SECOND *(GRAND_DELAY+rimeaddr_node_addr.u8[0]),generate_packet, NULL);
  }
	
    while (1)	{
		
		//Wait here for an event to happen
		PROCESS_WAIT_EVENT();
				

		// and loop
	}

  

  PROCESS_END();
}
void send_gradient() {
    
    
    buff[0]=My_Rank;
    
    packetbuf_clear();
    packetbuf_copyfrom(buff, BUFF_SIZE);
    broadcast_send(&broadcast);
    
    printf("Sending Gradient %d\n",My_Rank);
  
}


void generate_packet() {
  static struct ctimer timerCBR;
  rimeaddr_t receiver;

      buff[0]=rimeaddr_node_addr.u8[0];
      buff[1]=++seq;
  
      receiver.u8[0] = find_father(); // fatherID;
      receiver.u8[1] = 0;
      
      packetbuf_clear();
      packetbuf_copyfrom(buff, BUFF_SIZE);
      
      unicast_send(&uc, &receiver);
      
      ctimer_set(&timerCBR, CLOCK_SECOND*CBR_RATE, generate_packet, NULL);
      printf("Generate packet %d -- %d father %d \n",rimeaddr_node_addr.u8[0],seq,receiver.u8[0]);
  
}

//---------------------------------------------------------------------------------------
void add_neighbor(rimeaddr_t n_id,uint8_t n_rank) {

struct neighbor_rec *n = list_head(neighbor_list);
struct neighbor_rec *new ;
//Last_Father=n_id;
printf("Inserting \n");
	while(n != NULL) {
	
	    if	(n_id.u8[0] ==n->addr.u8[0]) {
	      
	      printf("Update \n");
	      if (n->rank>n_rank) {
		  n->rank=n_rank;
	      }
	      
	      return ;
	    }
	    n = list_item_next(n);
	  }
	  // new entry
	    new = memb_alloc(&neighbor_memb);    
	    if(new != NULL) {
		//Init neighbor entry
		printf("New \n");
		new->addr.u8[0]=n_id.u8[0];
		new->addr.u8[1]=0;
		new->rank=n_rank;
		list_add(neighbor_list, new);
		++num_recs;
	    }
    
  
}


void remove_neighbor(rimeaddr_t n_id) {
struct neighbor_rec *n = list_head(neighbor_list);
  
	while(n != NULL) {
	    if	(n_id.u8[0] ==n->addr.u8[0]) {
	      // remove_entry
	      list_remove(neighbor_list, n);
	      memb_free(&neighbor_memb, n);
	      return ;
	    }
	    n = list_item_next(n);
	    
	    --num_recs;
	  }
  
}

void remove_all() {
struct neighbor_rec *n,*p= list_head(neighbor_list);
  
	while(n != NULL) {
	  
	  p=n;
	  n = list_item_next(n);
	  list_remove(neighbor_list, n);
	  memb_free(&neighbor_memb, n);
	  --num_recs;
	  printf("remove \n");
	}
}
uint8_t find_father() {
struct neighbor_rec *n = list_head(neighbor_list);
// round robin 

uint8_t index=total_trans % num_recs,i=0,fID;
 printf ("Num REcs %d --- %d \n",num_recs,index);
	while(n != NULL) {
	  printf("%d ",n->addr.u8[0]);
	    if	(index==i) {
		fID=n->addr.u8[0];
	    }
	   i++;
	   n = list_item_next(n);
	   
	  }
	  printf(" \n");
	  ++total_trans;
	  return fID;
  
}



//---------------------------------------------------------------------------------------