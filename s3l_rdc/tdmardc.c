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
//#include "net/queuebuf.h"
#include <string.h>
#include "tic-toc.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7])
#else
#define PRINTF(...)
#define PRINTADDR(addr)
#endif


#define RTIMER_MS (RTIMER_SECOND/1000.0)

//Define FCF value
#define FRAME_TDMA_BEACONFRAME  (0x00)
#define FRAME_TDMA_DATAFRAME    (0x01)
#define FRAME_TDMA_CMDFRAME     (0x03)

#define FRAME_TDMA_BEACON_DST    (0x00)

struct tdma_hdr
{
  uint16_t	frame_type;
  uint8_t	seq_no;
  uint8_t	dst_node_id;
  uint8_t	src_node_id;
  uint8_t	payload_len;
};

#define MAC_HDR_SIZE 6		//MAC Header size
#define MAC_FT_SIZE 1		//MAC Footer size
//#define MAC_PL_SIZE 120		//MAC payload size
#define MAC_PL_SIZE 10
#define MAC_TOTAL_SIZE 17	//MAC total size

/*-----------------------------------------------*/
// Global Variables

// Scheudling
static uint16_t segment_period = 1000;   //one round time in ms
static uint16_t TS_period = 100;         //one time-slot duration in ms
static uint16_t BS_period = 100;         //BS broadcasts duration in ms

//slot information
static uint8_t total_slot_num;
static int8_t my_slot;

// packet format -- removed when using Zigbee frame
//static char pkt_hdr[] = {65,-120,-120,-51,-85,-1,-1, SN_ID, 0};
//#define PKT_HDR_SIZE 9
//#define NODE_INDEX   7
//#define SEQ_INDEX   8
#define FREE_SLOT_CONST 127

//static uint8_t pkt_size;
//char seq_num = 0;
static struct tdma_hdr frame_hdr;
//static char *pkt;

//buffer to store data from Application layer
static uint8_t buffer[MAC_PL_SIZE] = {0};
static uint8_t buf_ptr;		//index for starting to write
static uint8_t buf_send_ptr;	//index of starting to send
static uint8_t buf_full_flg = 0;	//flag to show if the buffer is full

static uint8_t buf_debug[3] = {0,1,2};



// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
static char *node_list;		//record all the input node

// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;

//debug ****
//const static float rtimer_ms = RTIMER_SECOND/1000.00;

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

  // update frame header
  frame_hdr.seq_no = frame_hdr.seq_no + 1;
   
   //pkt content
  memcpy(buffer,node_list,total_slot_num);
  
   //prepare frame for sending
  uint8_t tx_buffer[MAC_TOTAL_SIZE];
  memcpy(tx_buffer,&frame_hdr,sizeof(struct tdma_hdr));
  memcpy(tx_buffer+MAC_HDR_SIZE,buffer,frame_hdr.payload_len*sizeof(uint8_t));
  //TODO: checksum
  memcpy(tx_buffer+MAC_HDR_SIZE+MAC_PL_SIZE,0,MAC_FT_SIZE*sizeof(uint8_t));
   
   //reset node list
   memset(node_list,FREE_SLOT_CONST,total_slot_num);
   
   //reset payload buffer
   
   memset(buffer,0,MAC_PL_SIZE);
   buf_full_flg = 0;
   buf_ptr = 0;
   buf_send_ptr = 0;
   
   
   //send packet -- pushed to radio layer
   if(NETSTACK_RADIO.send(tx_buffer,MAC_TOTAL_SIZE) != RADIO_TX_OK)
       printf("TDMA RDC: BS fails to send packet\n");
   else
   {
       printf("TDMA RDC: BS sends %d\n",frame_hdr.seq_no);
       free(tx_buffer);
   }
   /*************************************************/
   /*
   //for debug circular buffer -- set MAC_PL_SIZE to 10
   buf_debug[0] += 3;
   buf_debug[1] += 3;
   buf_debug[2] += 3;
   packetbuf_copyfrom(buf_debug,3*sizeof(uint8_t));
   
    uint8_t data_len = packetbuf_datalen();
    uint8_t *ptr;
    ptr = (uint8_t *)packetbuf_dataptr();
    
    if((data_len+buf_ptr)<=MAC_PL_SIZE)
    {
      memcpy(buffer+buf_ptr,ptr,data_len*sizeof(uint8_t));
      buf_ptr = buf_ptr+data_len;
    }
    else
    {
      uint8_t temp_len = MAC_PL_SIZE-buf_ptr;
      memcpy(buffer+buf_ptr,ptr,temp_len*sizeof(uint8_t));
      buf_full_flg = 1;
      buf_ptr = 0;
      memcpy(buffer+buf_ptr,ptr+temp_len,(data_len-temp_len)*sizeof(uint8_t));
      buf_ptr = buf_ptr + data_len-temp_len;
    }
    
    if(buf_full_flg == 1)
      buf_send_ptr = buf_ptr;
    
    int i = 0;
    PRINTF("%d %d %d\n",buf_ptr,buf_send_ptr,data_len);
    for(i = 0; i < MAC_PL_SIZE; i++)
    {
      PRINTF("%d ",buffer[i]);
    }
    PRINTF("\n");
   */
  /*************************************************/
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

    //update header
    frame_hdr.seq_no = frame_hdr.seq_no+1;
    if (buf_full_flg == 0)
      frame_hdr.payload_len = buf_ptr;
    else
      frame_hdr.payload_len = MAC_PL_SIZE;

    //slot number
    //pkt[PKT_HDR_SIZE+1] = my_slot;
    
    //prepare frame for sending
    uint8_t tx_buffer[MAC_TOTAL_SIZE];
    memcpy(tx_buffer,&frame_hdr,sizeof(struct tdma_hdr));
    //TODO: checksum
    memcpy(tx_buffer+MAC_HDR_SIZE+MAC_PL_SIZE,0,MAC_FT_SIZE*sizeof(uint8_t));
    
    if (buf_full_flg == 0)
      memcpy(tx_buffer+MAC_HDR_SIZE,buffer,sizeof(uint8_t)*frame_hdr.payload_len);
    else
    {
      uint8_t temp_len = MAC_PL_SIZE - buf_send_ptr;
      memcpy(tx_buffer+MAC_HDR_SIZE,buffer+buf_send_ptr,sizeof(uint8_t)*temp_len);
      memcpy(tx_buffer+MAC_HDR_SIZE+temp_len,buffer,sizeof(uint8_t)*buf_send_ptr);
    }
    
    //reset payload buffer
    memset(buffer,0,MAC_PL_SIZE);
    buf_full_flg = 0;
    buf_ptr = 0;
    buf_send_ptr = 0;

    // send packet -- pushed to radio layer
    if(NETSTACK_RADIO.on())
    {
        if(NETSTACK_RADIO.send(tx_buffer,MAC_TOTAL_SIZE) != RADIO_TX_OK)
            printf("TDMA RDC: SN fails to send packet\n");
        else
	{
            printf("TDMA RDC: SN sends %d\n",frame_hdr.seq_no);
	    free(tx_buffer);
	}
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
    //copy sent data from packetbuf to local buffer. use circular buffer to manage
    //always return MAC_TX_OK

    uint8_t data_len = packetbuf_datalen();
    uint8_t *tx_ptr;
    tx_ptr = (uint8_t *)packetbuf_dataptr();
    
    //PRINTF("%d %d %d\n",tx_ptr[0],tx_ptr[1],tx_ptr[2]);
    
    if((data_len+buf_ptr)<=MAC_PL_SIZE)
    {
      memcpy(buffer+buf_ptr,tx_ptr,data_len*sizeof(uint8_t));
      buf_ptr = buf_ptr+data_len;
    }
    else
    {
      uint8_t temp_len = MAC_PL_SIZE-buf_ptr;
      memcpy(buffer+buf_ptr,tx_ptr,temp_len*sizeof(uint8_t));
      buf_full_flg = 1;
      buf_ptr = 0;
      memcpy(buffer+buf_ptr,tx_ptr+temp_len,(data_len-temp_len)*sizeof(uint8_t));
      buf_ptr = buf_ptr + data_len-temp_len;
    }
    
    if(buf_full_flg == 1)
      buf_send_ptr = buf_ptr;
/*    
    int i = 0;
    PRINTF("%d %d %d\n",buf_ptr,buf_send_ptr,data_len);
    for(i = 0; i < MAC_PL_SIZE; i++)
    {
      PRINTF("%d ",buffer[i]);
    }
    PRINTF("\n");
*/    
    
    

    //callback
    mac_call_sent_callback(sent_callback,ptr_callback,MAC_TX_OK,1);
   
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
    printf("in input\n");
    //parse frame
    uint8_t *rx_pkt = (uint8_t *)packetbuf_dataptr();
    //PRINTF("RX packet, size %d\n",strlen(rx_pkt));
    tic(RTIMER_NOW(),"pkt_in");

    
    struct tdma_hdr rx_hdr;
    memcpy(&rx_hdr,rx_pkt,sizeof(struct tdma_hdr));
    //TODO: set packetbuf attributes
    
    //if my packet
    if((rx_hdr.dst_node_id != SN_ID) && (rx_hdr.dst_node_id != FRAME_TDMA_BEACON_DST))
      return;
    
    uint8_t rx_buffer[MAC_PL_SIZE];
    memcpy(rx_buffer,rx_pkt+MAC_HDR_SIZE,sizeof(uint8_t)*rx_hdr.payload_len);
    
    //TODO:do checksum
    

    /*-------------SN CODE----------------------*/
    if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
    {
		
		
	toc();
	tic(RTIMER_NOW(),"pkt_in");        
	
	
	//PRINTF("From last RX %u\n",(RTIMER_NOW()-SN_RX_start_time)/RTIMER_MS);
	SN_RX_start_time = RTIMER_NOW();
	
	//check where the packet is from BS
       // if (rx_pkt[NODE_INDEX] != 0)
        //    return; //if not from BS, skip

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
        for(i = 0; i < rx_hdr.payload_len; i++)
        {
            //print payload
	    //PRINTF("%d ",rx_pkt[i]);
	    if(SN_ID == rx_buffer[i])
            {
                my_slot = i;
		break;
            }    
            else
            {
                if (rx_buffer[i] == FREE_SLOT_CONST) // exist free slot
                    free_slot++;

                //my_slot = -1;
            }
        }


        if (my_slot == -1 && free_slot != 0) //do not allocate a slot & there is a free slot
        {
            uint8_t rnd_num = RTIMER_NOW() % free_slot;
            for(i = 0; i<rx_hdr.payload_len; i++)
            {
                if(rx_buffer[i] == FREE_SLOT_CONST)
                {
                    if (rnd_num == 0)
                    {
                        my_slot = i;
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
	  node_list[current_TS] = rx_hdr.src_node_id;
	}
        PRINTF("[Sensor: %d] [Slot: %d] [Seq: %d]\n",
	       rx_hdr.src_node_id,current_TS,rx_hdr.seq_no);

    }

  //free local buffer
  
  //TODO: call NETSTACK_MAC.input()

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
    
    //debug
    //TDMA_SN_send();
    
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
    
    //allocate frame header
    
    
    if (SN_ID == 0)
    {
      frame_hdr.frame_type = FRAME_TDMA_BEACONFRAME;
      frame_hdr.seq_no = 0;
      frame_hdr.dst_node_id = FRAME_TDMA_BEACON_DST;
      frame_hdr.src_node_id = SN_ID;
      frame_hdr.payload_len = 0;
    }
    else
    {
      frame_hdr.frame_type = FRAME_TDMA_DATAFRAME;
      frame_hdr.seq_no = 0;
      frame_hdr.dst_node_id = 0;
      frame_hdr.src_node_id = SN_ID;
      frame_hdr.payload_len = 0;
    }
    
    
    
    // calculate the number of slots
    total_slot_num = (segment_period - BS_period)/TS_period;
    
    //allocate frame payload
    if (SN_ID == 0)
    {
      frame_hdr.payload_len = total_slot_num;
      memset(buffer,FREE_SLOT_CONST,total_slot_num);
    }
    else
    {
      memset(buffer,-1,MAC_PL_SIZE);
    }
   
    //limit the packetbuf data size to be the maximum of payload size
    packetbuf_set_datalen(MAC_PL_SIZE);
    buf_ptr = 0;
    buf_send_ptr = 0; 
    
    
    //allocate node_list space
    node_list = (char *)malloc(total_slot_num);
    memset(node_list,FREE_SLOT_CONST,total_slot_num);
    
    
    
    printf("Init RDC layer\n");

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
