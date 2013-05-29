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
#include <string.h>
#include "tic-toc.h"
#include "app.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#define RTIMER_MS (RTIMER_SECOND/1000.0)

/*-----------------------------------------------*/
// Global Variables

// Scheudling
static uint16_t segment_period = 1100;   //one round time in ms
static uint16_t TS_period = 100;         //one time-slot duration in ms
static uint16_t BS_period = 100;         //BS broadcasts duration in ms

//slot information
static uint8_t total_slot_num;
static int8_t my_slot;

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
static char *pkt;
static uint8_t pkt_size;
char seq_num = 0;

//struct tdma_hdr header;

// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
static char *node_list;		//record all the input node


// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static char buffer[10] = {0};
static uint8_t buf_ptr = 0;
static uint8_t buf_send_ptr = 0;
static uint8_t buf_full_flg = 0;


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;

//debug ****
const static float rtimer_ms = RTIMER_SECOND/1000.00;

// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{
    //PRINTF("From last send %u\n",(RTIMER_NOW()-BS_TX_start_time));
    BS_TX_start_time = RTIMER_NOW();
    BS_RX_start_time = BS_TX_start_time+BS_period*RTIMER_MS;
    //PRINTF("BS_TX_start_time %u\n",BS_TX_start_time);
    
    tic(RTIMER_NOW(),"BS");
    toc();
    tic(RTIMER_NOW(),"BS");
 


   // set timer for next BS send
   // right now, rtimer_timer does not consider drifting. For long time experiment, it may have problem
   uint16_t offset = RTIMER_MS*segment_period;
   //PRINTF("BS offset: %u\n",offset);
   rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+offset,0,TDMA_BS_send,NULL);
   
   
   
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
   memset(node_list,FREE_SLOT_CONST,total_slot_num);

   //send packet -- pushed to radio layer
   if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
       printf("TDMA RDC: BS fails to send packet\n");
   else
       printf("TDMA RDC: BS sends %d\n",pkt[SEQ_INDEX]);

   print_tics();
}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
    tic(RTIMER_NOW(),"SN send");
    
    
    //set timer for open RADIO -- 5ms for opening earlier
    //uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
    uint16_t time = RTIMER_NOW() + RTIMER_MS*(segment_period-BS_period-(my_slot+1)*TS_period - 5);
    rtimer_set(&SNTimer,time,0,NETSTACK_RADIO.on,NULL);

    pkt[SEQ_INDEX] = seq_num++;

    //slot number
    pkt[PKT_HDR_SIZE+1] = my_slot;
    
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

 //   memcpy(pkt+PKT_HDR_SIZE,buffer,sizeof(uint8_t)*10);
    //reset payload
    //memset(buffer,0,10);
//    buf_full_flg = 0;
//    buf_ptr = 0;
//    buf_send_ptr = 0;

    // send packet -- pushed to radio layer
    if(NETSTACK_RADIO.on())
    {
        if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
            printf("TDMA RDC: SN fails to send packet\n");
        else
            printf("TDMA RDC: SN sends %d\n",pkt[SEQ_INDEX]);
    }
    else
        printf("TDMA RDC: SN fails to open radio\n");

    // turn off radio
    NETSTACK_RADIO.off();

    

}

/*-----------------------------------------------*/
// send packet -- not used in TDMA
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
  
 /* 
  int i = 0;
  for(i = 0; i < 10; i++)
  {
    PRINTF("%d ",buffer[i]);
  }
  PRINTF("\n");
  */
 
}
/*-----------------------------------------------*/
// send packet list -- not used in TDMA
static void send_list(mac_callback_t sent_callback, void *ptr, struct rdc_buf_list *list)
{
}
/*-----------------------------------------------*/
// receives packet -- called in radio.c,radio.h
static void input(void)
{
    
    char *rx_pkt = (char *)packetbuf_dataptr();
    //PRINTF("RX packet, size %d\n",strlen(rx_pkt));
    tic(RTIMER_NOW(),"pkt_in");

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
		
		
	toc();
	tic(RTIMER_NOW(),"pkt_in");        
	
	
	//PRINTF("From last RX %u\n",(RTIMER_NOW()-SN_RX_start_time)/RTIMER_MS);
	SN_RX_start_time = RTIMER_NOW();
	
	//check where the packet is from BS
        if (rx_pkt[NODE_INDEX] != 0)
            return; //if not from BS, skip

        /*--------from BS------------*/
        
        //turn off radio -- save power
        if(NETSTACK_RADIO.off() != 1)
        {
            printf("TDMA RDC: SN fails to turn off radio");
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
	
	print_tics();
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
	  
	}
	

	
        PRINTF("[Sensor: %d] [Slot: %d] [Seq: %d]\n",
	       rx_pkt[NODE_INDEX],current_TS,rx_pkt[SEQ_INDEX]);


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
	
	
    
    
    
/*    
    if (SN_ID == 0)
    {
      header.frame_type = FRAME_TDMA_BEACONFRAME;
      header.seq_num = 0;
      header.dst_node_id = FRAME_TDMA_BEACON_DST;
      header.src_node_id = 0;
      header.payload_len = 0;
    }
    else
    {
      header.frame_type = FRAME_TDMA_DATAFRAME;
      header.seq_num = 0;
      header.dst_node_id = 0;
      header.src_node_id = SN_ID;
      header.payload_len = 0;
    }
*/	
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
