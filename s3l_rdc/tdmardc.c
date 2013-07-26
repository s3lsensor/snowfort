/**
* \file 
* 	A TDMA RDC implementation
* \author
* 	Yizheng Liao <yzliao@stanford.edu>
*/

#include "net/mac/tdmardc.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "sys/rtimer.h"
#include "net/queuebuf.h"
#include "dev/cc2420.h"
#include <string.h>
//#include "tic-toc.h"
#include <stdio.h>



#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


//#define RTIMER_MS 4// actually .9766 ms, if rtimer_second = 4096
#define RTIMER_MS 33 // rtimer_second = 32768, closest integer estimate of ms. actually 1.0071 ms
//#define RTIMER_MS (RTIMER_SECOND/1000.0) // unwise to play with floats...

/*-----------------------------------------------*/
// Global Variables

// Scheudling
static uint16_t segment_period = 1100;   //one round time in ms
static uint16_t TS_period = 100;         //one time-slot duration in ms
static uint16_t BS_period = 100;         //BS broadcasts duration in ms

//slot information
static uint8_t total_slot_num; // calculated in init()
static int8_t my_slot; // set in SN_send

// packet format -- removed when using Zigbee frame
static char pkt_hdr[] = {65,-120,-120,-51,-85,-1,-1, SN_ID, 0};
//FCF: 65,-120
//Data Sequence number: -120
//Address information: -51 -85(CD AB in hex) -1 -1 (short address mode)
//Actually in the payload: SN_ID,0

#define PKT_HDR_SIZE 9
#define NODE_INDEX   7
#define SEQ_INDEX   8
#define FREE_SLOT_CONST 127
static char *pkt; //allocated in init()
static uint8_t pkt_size; //set in init()
char seq_num = 10; //any reason why it defaults at 10?

//struct tdma_hdr header;

// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
static char *node_list; //allocated, initialized in init()


// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static uint16_t radioontime;
static uint16_t radiodelay;
static char buffer[10] = {0};
static uint8_t buf_ptr = 0; //updated when send() called (RDC_send()) directly
static uint8_t buf_send_ptr = 0; //updated when send() called (RDC_send()) directly
static uint8_t buf_full_flg = 0; //updated when send() called RDC_send()) directly

static int rx_finished_flag = 0;

int check_rx_finished(void);

//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;

//debug ****
//const static float rtimer_ms = RTIMER_SECOND/1000.00; //unused

int check_rx_finished(void)
{
    return rx_finished_flag;
}

// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{
    //PRINTF("From last send %u\n",(RTIMER_NOW()-BS_TX_start_time));
    BS_TX_start_time = RTIMER_NOW();
    BS_RX_start_time = BS_TX_start_time+BS_period*RTIMER_MS;
    //PRINTF("BS_TX_start_time %u\n",BS_TX_start_time);
    
    //tic(RTIMER_NOW(),"BS");
    //toc();
    //tic(RTIMER_NOW(),"BS");



  // set timer for next BS send
  // right now, rtimer_timer does not consider drifting. For long time experiment, it may have problem
  uint16_t offset = RTIMER_MS*(segment_period);
  //PRINTF("BS offset: %u\n",offset);
  rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+offset,0,TDMA_BS_send,NULL); // replace with BS_TX_START?
  
  
  
  //pkt content
  pkt[SEQ_INDEX] = seq_num++;
  memcpy(pkt+PKT_HDR_SIZE,node_list,total_slot_num);
/*
  int i = 0;
  for (i = 0; i < total_slot_num; i++)
  {
      PRINTF("%d ",node_list[i]);
  }
  PRINTF("\n");
*/   
  //memset(node_list,FREE_SLOT_CONST,total_slot_num);

  //send packet -- pushed to radio layer
  if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
      printf("TDMA RDC: BS fails to send packet\n");
  else
      printf("TDMA RDC: BS sends %d\n",pkt[SEQ_INDEX]);
  //print_tics();
}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
    //tic(RTIMER_NOW(),"SN send");
    
    
    //set timer for open RADIO -- for opening earlier // originally 5 ms (insufficient? no, on order of 2ms delay)
    //uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
    radioontime = RTIMER_NOW() + RTIMER_MS*(segment_period-BS_period-(my_slot)*TS_period - 5); //replace 5 with #def? // was originally my_slot+1. This is definitely wrong (creates bug where last slot has huge positive wait for radio turn-on)
    rtimer_set(&SNTimer,radioontime,0,NETSTACK_RADIO.on,NULL);

    pkt[SEQ_INDEX] = seq_num++;

    //slot number
    pkt[PKT_HDR_SIZE+1] = my_slot; // equivalent to payload(1)???
    
    //copy header
    //header.seq_num = seq_num;
    
    //memcpy(pkt+PKT_HDR_SIZE,&header,sizeof(struct tdma_hdr));
    
    if (buf_full_flg == 0)
      memcpy(pkt+PKT_HDR_SIZE,buffer,sizeof(uint8_t)*buf_ptr);
    else
    {
      uint8_t temp_len = 10 - buf_send_ptr;
      memcpy(pkt+PKT_HDR_SIZE,buffer+buf_send_ptr,sizeof(uint8_t)*temp_len);
      memcpy(pkt+PKT_HDR_SIZE+temp_len,buffer,sizeof(uint8_t)*buf_send_ptr);
    }
    //buf_full_flg is never reset? it should be reset in case 
    //that buffer length and buf_send_ptr align, which set buf_ptr and 
    //buf_send_ptr back to 0.


//   memcpy(pkt+PKT_HDR_SIZE,buffer,sizeof(uint8_t)*10);
    //reset payload
    //memset(buffer,0,10);
//    buf_full_flg = 0;
//    buf_ptr = 0;
//    buf_send_ptr = 0;

    pkt[PKT_HDR_SIZE+6] = (char)((radiodelay>>8) & 0xFF);
    pkt[PKT_HDR_SIZE+7] = (char)(radiodelay & 0xFF);



    // send packet -- pushed to radio layer
    if(NETSTACK_RADIO.on()){
	if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK){
	    #if DEBUG
	    printf("TDMA RDC: SN fails to send packet\n");
	    #endif
	}else{
	    #if DEBUG      
	    printf("TDMA RDC: SN sends %d\n",pkt[SEQ_INDEX]);
	    #endif
	}
    }else{
	#if DEBUG
	printf("TDMA RDC: SN fails to open radio\n");
	#endif
     }
    // turn off radio
    NETSTACK_RADIO.off();
}

/*-----------------------------------------------*/
// send packet -- not used in TDMA
static void send(mac_callback_t sent_callback, void *ptr_callback)
{
  uint8_t data_len = packetbuf_datalen();
  //uint16_t timenowsend = RTIMER_NOW();
  //printf("RDC_SEND called. TIME: %d\n", timenowsend);
  uint8_t *ptr;
  ptr = (uint8_t *)packetbuf_dataptr();
  
  if((data_len+buf_ptr) <= 10)
  {
    memcpy(buffer+buf_ptr,ptr,data_len*sizeof(uint8_t));
    buf_ptr = buf_ptr + data_len;
  }
  else
  {
    uint8_t temp_len = 10-buf_ptr;
    memcpy(buffer+buf_ptr,ptr,temp_len*sizeof(uint8_t));
    buf_full_flg = 1;
    buf_ptr = 0;
    memcpy(buffer+buf_ptr,ptr+temp_len,(data_len-temp_len)*sizeof(uint8_t));
    buf_ptr = buf_ptr + data_len - temp_len;
    
  }
  
  if(buf_full_flg == 1)
    buf_send_ptr = buf_ptr;
  
 
/*  int i = 0;
  for(i = 0; i < 10; i++)
  {
    PRINTF("%d ",buffer[i]);
  }
  PRINTF("\n");*/
  

}
/*-----------------------------------------------*/
// send packet list -- not used in TDMA
static void send_list(mac_callback_t sent_callback, void *ptr, struct rdc_buf_list *list)
{
    #if DEBUG
    printf("SEND_LIST NOT CALLED");
    #endif
}
/*-----------------------------------------------*/
// receives packet -- called in radio.c,radio.h
static void input(void)
{
    
    char *rx_pkt = (char *)packetbuf_dataptr();//add read of rx_len?
    //PRINTF("RX packet, size %d\n",strlen(rx_pkt));
    //tic(RTIMER_NOW(),"pkt_in");

    //debug
    /*
    short j = 0;
    for(j = 0; j < pkt_size; j++)
    {
	PRINTF("%d ",rx_pkt[j]);
    }
    PRINTF("\n");
    */

    /*-------------SN CODE----------------------*/
    if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
    {
		
		
	//toc();
	//tic(RTIMER_NOW(),"pkt_in");        
	
	
	//PRINTF("From last RX %u\n",(RTIMER_NOW()-SN_RX_start_time)/RTIMER_MS);
	SN_RX_start_time = RTIMER_NOW();
	
	//check where the packet is from BS
	if (rx_pkt[NODE_INDEX] != 0)
	    return; //if not from BS, skip. turn off radio? perhaps, but there are bigger problems if that's happening...

	/*--------from BS------------*/
	
	//turn off radio -- save power
	if(NETSTACK_RADIO.off() != 1)
	{
	    #if DEBUG
	    printf("TDMA RDC: SN fails to turn off radio");
	    #endif
	}

	


	//first, check if BS assigns a slot

	char i = 0;
	char free_slot = 0;
	my_slot = -1;
	for(i = PKT_HDR_SIZE; i < pkt_size; i++)
	{
	    //print payload
	    //PRINTF("%d ",rx_pkt[i]);
	    if(SN_ID == rx_pkt[i])
	    {
		my_slot = i-PKT_HDR_SIZE;
		break;
	    }    
	    else
	    {
		if (rx_pkt[i] == FREE_SLOT_CONST) // exist free slot
		    free_slot++;

		//my_slot = -1;
	    }
	}


	if (my_slot == -1 && free_slot != 0) //do not allocate a slot & there is a free slot
	{
	    uint8_t rnd_num = RTIMER_NOW() % free_slot;
	    for(i = PKT_HDR_SIZE; i<pkt_size; i++)
	    {
		if(rx_pkt[i] == FREE_SLOT_CONST)
		{
		    if (rnd_num == 0)
		    {
			my_slot = i-PKT_HDR_SIZE;
			break;
		    }
		    else
			rnd_num--;
		}
	    }            
	}

	//schedule for TX -- 5ms for guarding period (open radio earlier)
	
	if (my_slot != -1)
	{
		//PRINTF("Schedule for TX at Slot %d\n",my_slot);
		uint16_t SN_TX_time = SN_RX_start_time + RTIMER_MS*(BS_period+TS_period * my_slot - 2); 
		rtimer_set(&SNTimer,SN_TX_time,0,TDMA_SN_send,NULL);
	}
        radiodelay = SN_RX_start_time - radioontime;
	rx_finished_flag = 1;
	//print_tics();
    }
    else if(SN_ID == 0) //BS
    /*-----------------BS CODE---------------*/
    {
	//tic(RTIMER_NOW(),"pkt_in");        
	
	//debug for collision
	/*
	short i = 0;
	for(i = 0; i < pkt_size; i++)
	{
	    PRINTF("%d ",rx_pkt[i]);
	}
	PRINTF("\n");
	*/
	//set flag in pkt for TS occupancy
	uint8_t current_TS = (RTIMER_NOW()-BS_RX_start_time)/(TS_period*RTIMER_MS);
	if(node_list[current_TS] == FREE_SLOT_CONST) //collision -- ask the node to find a new available slot
	{
	  //pkt[PKT_HDR_SIZE+current_TS] = rx_pkt[NODE_INDEX];
	  node_list[current_TS] = rx_pkt[NODE_INDEX];
	  //struct tdma_hdr rx_hdr;
	  //memcpy(&rx_hdr,rx_pkt+PKT_HDR_SIZE,sizeof(struct tdma_hdr));
	  //PRINTF("HDR: From node %d, seq_num %d\n",rx_hdr.src_node_id,rx_hdr.seq_num);
	  /*
	  if (rx_pkt[NODE_INDEX] == 1 || rx_pkt[NODE_INDEX] == 10)
	  {
	      uint8_t i = 0;
	      PRINTF("DATA:");
	      for(i = 0; i < 10; i++)
	      {
		PRINTF("%d ",rx_pkt[PKT_HDR_SIZE+i]);
	      }
	      PRINTF("\n");
	  }
	  */
	  
	}
	

	
	printf("[Sensor: %d] [Slot: %d] [Seq: %d]\n",
	       rx_pkt[NODE_INDEX],current_TS,rx_pkt[SEQ_INDEX]);

	printf("Channel: %d\n", cc2420_get_channel());
	printf("RSSI: %d\n", cc2420_last_rssi-45);

    	uint8_t measurevector[6];
    	memcpy(measurevector,rx_pkt+PKT_HDR_SIZE,6*sizeof(uint8_t));
	uint16_t accx,accy,accz;

	accx = 0;
	accx |= measurevector[0];
	accx = (accx<<8) | measurevector[1];
	accy = 0;
	accy |= measurevector[2];
	accy = (accy<<8) | measurevector[3];
	accz = 0;
	accz |= measurevector[4];
	accz = (accz<<8) | measurevector[5];

	printf("Accel: %d %d %d\n", accx, accy, accz);

    }


}
/*-----------------------------------------------*/
// turn on RDC layer
static int on(void)
{
    PRINTF("turn on RDC layer\n");
    if (SN_ID == 0) //BS sends packet
    {
	rtimer_set(&BSTimer,RTIMER_NOW()+RTIMER_MS*segment_period,0,TDMA_BS_send,NULL);
    }
    return NETSTACK_RADIO.on();
}
/*-----------------------------------------------*/
// turn off RDC layer
static int off(int keep_radio_on)
{
    if (keep_radio_on)
	return NETSTACK_RADIO.on();
    else
	return NETSTACK_RADIO.off();
}
/*-----------------------------------------------*/
// check channel -- do not need in TDMA implementation
static unsigned short channel_check_interval(void)
{
	return 0;
}
/*-----------------------------------------------*/
// initalize RDC layer
static void init(void)
{
  
    printf("node id %d\n",SN_ID); 
    /*uint16_t debugtime0 = RTIMER_NOW();
    uint16_t debugtime1 = RTIMER_NOW()+RTIMER_SECOND;
    uint16_t debugtime2 = RTIMER_NOW()+RTIMER_SECOND*2;
    printf("TIME DEBUG: %d %d %d\n", debugtime0, debugtime1, debugtime2);*/


    // calculate the number of slots
    total_slot_num = (segment_period - BS_period)/TS_period;
    
    //allocate node_list space
    node_list = (char *)malloc(total_slot_num);
    memset(node_list,FREE_SLOT_CONST,total_slot_num);
    
    // allocate packet space
    pkt_size = PKT_HDR_SIZE + total_slot_num;
    pkt = ( char *)malloc(pkt_size+1);
    memcpy(pkt,pkt_hdr,PKT_HDR_SIZE);
    if (SN_ID == 0)
	memset(pkt+PKT_HDR_SIZE,FREE_SLOT_CONST,total_slot_num); //set free slot
    else
	memset(pkt+PKT_HDR_SIZE,-1,total_slot_num); //free payload for SN
	
	
    printf("Init RDC layer,packet size\n");

    on();
}

/*-----------------------------------------------*/


/*-----------------------------------------------*/
const struct rdc_driver tdmardc_driver = 
{
	"tdmardc",
	init, 		//Initialize the RDC driver
	send, 		//send a packet from the rime buffer
	send_list,	//send a packet list
	input,		//callback for getting notified of incoming packet
	on,		//turn the RDC layer on
	off,		//turn the RDC layer off
	channel_check_interval,		//returns the channel check interval,expressed in clock_time_t tick
	
};
/*-----------------------------------------------*/
