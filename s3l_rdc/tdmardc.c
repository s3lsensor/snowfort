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
#include <stdio.h>




#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


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
#define FREE_SLOT_CONST 0x7F
static char *pkt; //allocated in init()
static uint8_t pkt_size; //set in init()
char seq_num = 0; 

// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
static char *node_list; //allocated, initialized in init()


// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static uint16_t radioontime;
static char buffer[10] = {0};
static uint8_t buf_ptr = 0; //updated when send() called (RDC_send()) directly
static uint8_t buf_send_ptr = 0; //updated when send() called (RDC_send()) directly
static uint8_t buf_full_flg = 0; //updated when send() called RDC_send()) directly


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;


// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{
    BS_TX_start_time = RTIMER_NOW();
    BS_RX_start_time = BS_TX_start_time+BS_period*RTIMER_MS;


  // set timer for next BS send
  // right now, rtimer_timer does not consider drifting. For long time experiment, it may have problem
  uint16_t offset = RTIMER_MS*(segment_period);
  //PRINTF("BS offset: %u\n",offset);
  rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+offset,0,TDMA_BS_send,NULL);
  
  
  
  //pkt content
  pkt[SEQ_INDEX] = seq_num++;
  memcpy(pkt+PKT_HDR_SIZE,node_list,total_slot_num);


  //send packet -- pushed to radio layer
  if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
      printf("TDMA RDC: BS fails to send packet\n");
  else
      printf("TDMA RDC: BS sends %d\n",pkt[SEQ_INDEX]);
}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
    //tic(RTIMER_NOW(),"SN send");
    
    
    //set timer for open RADIO -- for opening earlier // originally 5 ms (insufficient? no, on order of 2ms delay)
    //uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
    radioontime = RTIMER_TIME(&SNTimer) + RTIMER_MS*(segment_period-BS_period-(my_slot)*TS_period - 5); //replace 5 with #def? // was originally my_slot+1. This is definitely wrong (creates bug where last slot has huge positive wait for radio turn-on)
    rtimer_set(&SNTimer,radioontime,0,NETSTACK_RADIO.on,NULL);

    pkt[SEQ_INDEX] = seq_num++;
    
    if (buf_full_flg == 0)
      memcpy(pkt+PKT_HDR_SIZE,buffer,sizeof(uint8_t)*buf_ptr);
    else
    {
      uint8_t temp_len = 10 - buf_send_ptr;
      memcpy(pkt+PKT_HDR_SIZE,buffer+buf_send_ptr,sizeof(uint8_t)*temp_len);
      memcpy(pkt+PKT_HDR_SIZE+temp_len,buffer,sizeof(uint8_t)*buf_send_ptr);
    }


    // send packet -- pushed to radio layer
    if(NETSTACK_RADIO.on()){
	if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK){
	    printf("TDMA RDC: SN fails to send packet\n");
	}else{
	    PRINTF("TDMA RDC: SN sends %d\n",pkt[SEQ_INDEX]);
	}
    }else{
	printf("TDMA RDC: SN fails to open radio\n");
     }
    // turn off radio
    NETSTACK_RADIO.off();
}

/*-----------------------------------------------*/
// send packet
static void send(mac_callback_t sent_callback, void *ptr_callback)
{
  uint8_t data_len = packetbuf_datalen();

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

  

}
/*-----------------------------------------------*/
// send packet list -- not used in TDMA
static void send_list(mac_callback_t sent_callback, void *ptr, struct rdc_buf_list *list)
{
    PRINTF("SEND_LIST NOT CALLED");
}
/*-----------------------------------------------*/
// receives packet -- called in radio.c,radio.h
static void input(void)
{
    
    char *rx_pkt = (char *)packetbuf_dataptr();//add read of rx_len?

    /*-------------SN CODE----------------------*/
    if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
    {
	SN_RX_start_time = RTIMER_NOW();
	
	//check where the packet is from BS
	if (rx_pkt[NODE_INDEX] != 0)
	    return; 

	/*--------from BS------------*/
	
	//turn off radio -- save power
	if(NETSTACK_RADIO.off() != 1)
	{
	    printf("TDMA RDC: SN fails to turn off radio");
	}

	//first, check if BS assigns a slot

	unsigned char i = 0;
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
    }
    else if(SN_ID == 0) //BS
    /*-----------------BS CODE---------------*/
    {
	//set flag in pkt for TS occupancy
	uint8_t current_TS = (RTIMER_NOW()-BS_RX_start_time)/(TS_period*RTIMER_MS);
	if(node_list[current_TS] == FREE_SLOT_CONST) //collision -- ask the node to find a new available slot
	{
	  node_list[current_TS] = rx_pkt[NODE_INDEX];	  
	}
	

	
	PRINTF("[Sensor: %d] [Slot: %d] [Seq: %d]\n",
	       rx_pkt[NODE_INDEX],current_TS,rx_pkt[SEQ_INDEX]);

	PRINTF("Channel: %d;", cc2420_get_channel());
	PRINTF("RSSI: %d\n", cc2420_last_rssi-45);

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
  
    printf("mote id %d\n",SN_ID); 

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
	memset(pkt+PKT_HDR_SIZE,(uint8_t)0x7F,total_slot_num); //free payload for SN
	
	
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