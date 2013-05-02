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
static uint16_t segment_period = 500;   //one round time in ms
static uint16_t TS_period = 50;         //one time-slot duration in ms
static uint16_t BS_period = 50;         //BS broadcasts duration in ms

//slot information
static uint8_t total_slot_num;
static uint8_t my_slot;

// packet format -- removed when using Zigbee frame
static char pkt_hdr[] = {65,-120,-120,-51,-85,-1,-1, SN_ID, 0};
#define PKT_HDR_SIZE 9
#define NODE_INDEX   7
#define SEQ_INDEX   8
static char *pkt;
static uint8_t pkt_size;
char seq_num = 0;


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;

// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{
   // set timer for next BS send
   // right now, rtimer_timer does not consider drifting. For long time experiment, may have problem
   rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+RTIMER_MS*segment_period,0,TDMA_BS_send,NULL);

   pkt[SEQ_INDEX] = seq_num++;

   //send packet -- pushed to radio layer
   if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
       printf("TDMA RDC: BS fails to send packet\n");
   else
       printf("TDMA RDC: BS sends %d\n",pkt[SEQ_INDEX]);
}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
    //set timer for open RADIO
    uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*((total_slot_num - my_slot -1)*TS_period);
    rtimer_set(&SNTimer,time,0,NETSTACK_RADIO.on,NULL);

    pkt[SEQ_INDEX] = seq_num++;

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
static void send(mac_callback_t sent_callback, void *ptr)
{
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
    PRINTF("RX packet, size %d\n",strlen(rx_pkt));

    /*-------------SN CODE----------------------*/
    if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
    {
        //check where the packet is from BS
        if (rx_pkt[NODE_INDEX] != 0)
            return; //if not from BS, skip

        /*--------from BS------------*/
        
        //turn off radio -- save power
        if(!NETSTACK_RADIO.off())
        {
            printf("TDMA RDC: SN fails to turn off radio");
        }

        my_slot = 4;
        //first, check if BS assigns a slot
/*
        char i = 0;
        char free_slot = 0;
        for(i = PKT_HDR_SIZE; i < pkt_size; i++)
        {
            if(SN_ID == rx_pkt[i])
            {
                my_slot = i-PKT_HDR_SIZE;
            }    
            else
            {
                if (rx_pkt[i] == 127) // exist free slot
                    free_slot++;

                my_slot = -1;
            }
        }


        if (my_slot == -1 && free_slot != 0) //do not allocate a slot & there is a free slot
        {
            
        }
*/
        //schedule for TX -- 5ms for guarding period (open radio earlier)
        uint16_t SN_TX_time = RTIMER_NOW() + RTIMER_MS*(BS_period + TS_period * my_slot - 5); 
        rtimer_set(&SNTimer,SN_TX_time,0,TDMA_SN_send,NULL);
    }
    else //BS
    /*-----------------BS CODE---------------*/
    {
        //debug for collision
        short i = 0;
        for(i = 0; i < pkt_size; i++)
        {
            PRINTF("%d ",rx_pkt[i]);
        }
        PRINTF("\n");
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
    
    // allocate packet space
    pkt_size = PKT_HDR_SIZE + total_slot_num;
    pkt = ( char *)malloc(pkt_size+1);
    memcpy(pkt,pkt_hdr,PKT_HDR_SIZE);
    if (SN_ID == 0)
        memset(pkt+PKT_HDR_SIZE,127,total_slot_num); //set free slot
    else
        memset(pkt+PKT_HDR_SIZE,-1,total_slot_num); //free payload for SN
    
    short i = 0;
    for(i = 0; i < pkt_size; i++)
        PRINTF("%d ",pkt[i]);
    PRINTF("\n");

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
