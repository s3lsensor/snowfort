/**
 * \file
 * 	A TDMA RDC implementation
 * \author
 * 	Yizheng Liao <yzliao@stanford.edu>
 */

#include "net/mac/tdmardc.h"
#include "lib/assert.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "sys/rtimer.h"
#include "net/queuebuf.h"
#include "dev/cc2420.h"
#include "appconn/app_conn.h"
#include <string.h>
#include <stdio.h>



#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*-----------------------------------------------*/
// Global Variables

// Scheudling
static const uint16_t segment_period = SEGMENT_PERIOD;   //one round time in ms
static const uint16_t TS_period = TS_PERIOD;         //one time-slot duration in ms
static const uint16_t BS_period = BS_PERIOD;         //BS broadcasts duration in ms

//slot information
static const uint16_t total_slot_num = TOTAL_TS; // calculated in init()
static int16_t my_slot=SLOT_NUM; // set in SN_send

//packet information
static char *pkt; //allocated in init()
static const uint8_t pkt_size = PKT_HDR_SIZE + MAX_PKT_PAYLOAD_SIZE; //set in init()
char seq_num = 0; 

// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
volatile rtimer_clock_t radio_TX_time;
static char *node_list; //allocated, initialized in init()


// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static uint16_t radioontime;
static char buffer[MAX_PKT_PAYLOAD_SIZE] = {0};
volatile static uint8_t buf_ptr = 0; //updated when send() called (RDC_send()) directly
volatile static uint8_t buf_send_ptr = 0; //updated when send() called (RDC_send()) directly
volatile static uint8_t buf_full_flg = 0; //updated when send() called RDC_send()) directly


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;


// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{
	//printf("%05u,",RTIMER_NOW());//call time for BS_send

	uint8_t bkn_len = 16;
	uint8_t bkn_pkt[16]={0};
	bkn_pkt[14]=1;
	bkn_pkt[15]=2;

	rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+segment_period,0,TDMA_BS_send,NULL);
	//pkt content
	pkt[SEQ_INDEX] = seq_num++;
	//pkt[PKT_PAYLOAD_SIZE_INDEX] = total_slot_num;
	//memcpy(pkt+PKT_HDR_SIZE,node_list,total_slot_num);
	pkt[PKT_PAYLOAD_SIZE_INDEX] = bkn_len;
	memcpy(pkt+PKT_HDR_SIZE,bkn_pkt,bkn_len);

	BS_TX_start_time = RTIMER_NOW();//packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP);//RTIMER_NOW();
	//printf("Bkn sent, t = %u, Timestamp = %u\n",BS_TX_start_time,packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP));
	// set timer for next BS send
	// right now, rtimer_timer does not consider drifting. For long time experiment, it may have problem
	//uint16_t offset = (segment_period);//*RTIMER_MS
	//PRINTF("BS offset: %u\n",offset);

	packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE, PACKETBUF_ATTR_PACKET_TYPE_TIMESTAMP);
	//send packet -- pushed to radio layer
	if(NETSTACK_RADIO.send(pkt,bkn_len+PKT_HDR_SIZE) != RADIO_TX_OK)
		PRINTF("TDMA RDC: BS fails to send packet\n");
	else
		PRINTF("TDMA RDC: BS sends beacon with Seq # %u\n",pkt[SEQ_INDEX]);

	//printf("%05u\n",radio_TX_time);
	BS_RX_start_time = radio_TX_time+BS_period;//*RTIMER_MS;
	printf("%lu,%lu\n",energest_type_time(ENERGEST_TYPE_LISTEN),energest_type_time(ENERGEST_TYPE_TRANSMIT));

	//energest_flush();

}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
	//tic(RTIMER_NOW(),"SN send");

	uint16_t callBkTime = RTIMER_NOW();
	//set timer for open RADIO -- for opening earlier 2 ms
	//uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
	radioontime = SN_RX_start_time+segment_period-GRD_PERIOD;//RTIMER_TIME(&SNTimer) + (total_slot_num-my_slot)*TS_period-GRD_PERIOD;//(segment_period-BS_period-(my_slot)*TS_period - GRD_PERIOD);
	rtimer_set(&SNTimer,radioontime,0,NETSTACK_RADIO.on,NULL);


	//printf("%05u,\n",radioontime);
	//uint16_t timeStmp = packetbuf_attr(PACKETBUF_ATTR_RADIO_TXPOWER);
	//printf("\nNext transmission at %u, Last pkt time = %u\n",radioontime, timeStmp);

	pkt[SEQ_INDEX] = seq_num++;


	if (buf_full_flg == 0)
	{
		memcpy(pkt+PKT_HDR_SIZE,buffer,sizeof(uint8_t)*buf_ptr);
		pkt[PKT_PAYLOAD_SIZE_INDEX] = buf_ptr;
	}
	else
	{
		uint8_t temp_len = MAX_PKT_PAYLOAD_SIZE - buf_send_ptr;
		memcpy(pkt+PKT_HDR_SIZE,buffer+buf_send_ptr,sizeof(uint8_t)*temp_len);
		memcpy(pkt+PKT_HDR_SIZE+temp_len,buffer,sizeof(uint8_t)*buf_send_ptr);
		pkt[PKT_PAYLOAD_SIZE_INDEX] = MAX_PKT_PAYLOAD_SIZE;
	}

	//uint16_t codeExeTime = RTIMER_NOW()-callBkTime;

	// send packet -- pushed to radio layer
	//packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE, PACKETBUF_ATTR_PACKET_TYPE_TIMESTAMP);
	//printf("%lu,",RTIMER_NOW());
	if(NETSTACK_RADIO.on())
	{
		//printf("%lu,\n",RTIMER_NOW());
		//if(NETSTACK_RADIO.send(pkt,pkt_size) != RADIO_TX_OK)
		if(NETSTACK_RADIO.send(pkt,buf_ptr+PKT_HDR_SIZE) != RADIO_TX_OK)
		{
			PRINTF("TDMA RDC: SN fails to send packet\n");
		}else
		{
			PRINTF("TDMA RDC: SN sends %d\n",pkt[SEQ_INDEX]);
		}
		buf_full_flg = 0;
		buf_ptr = 0;
		buf_send_ptr = 0;
	}
	else
	{
		PRINTF("TDMA RDC: SN fails to open radio\n");
	}
	// turn off radio
	NETSTACK_RADIO.off();
	//printf("%05u,",callBkTime);//callback time SN_send
	//printf("%05u\n",radio_TX_time);

	printf("%lu,%lu\n",energest_type_time(ENERGEST_TYPE_LISTEN),energest_type_time(ENERGEST_TYPE_TRANSMIT));
	//energest_flush();
	//printf("\n");
}

/*-----------------------------------------------*/
// send packet
static void send(mac_callback_t sent_callback, void *ptr_callback)
{
	uint8_t data_len = packetbuf_datalen();

	uint8_t *ptr;
	ptr = (uint8_t *)packetbuf_dataptr();

	if((data_len+buf_ptr) <= MAX_PKT_PAYLOAD_SIZE)
	{
		memcpy(buffer+buf_ptr,ptr,data_len*sizeof(uint8_t));
		buf_ptr = buf_ptr + data_len;
	}
	else
	{
		uint8_t temp_len = MAX_PKT_PAYLOAD_SIZE-buf_ptr;
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



	/*-------------SN CODE----------------------*/
	if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
	{
		//printf("Last receive = %u, current = %u\n",SN_RX_start_time, packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP));

		//turn off radio -- save power
		if(NETSTACK_RADIO.off() != 1)
		{
			PRINTF("TDMA RDC: SN fails to turn off radio");
		}

		char *rx_pkt = (char *)packetbuf_dataptr();
		uint16_t rx_pkt_len = rx_pkt[PKT_PAYLOAD_SIZE_INDEX];

		SN_RX_start_time = packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP);


		//check where the packet is from BS
		if (rx_pkt[NODE_INDEX] != 0)
			return;

		/*--------from BS------------*/


		/*//first, check if BS assigns a slot

		unsigned char i = 0;
		char free_slot = 0;
		my_slot = -1;
		for(i = PKT_HDR_SIZE; i < PKT_HDR_SIZE+rx_pkt_len; i++)
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
			for(i = PKT_HDR_SIZE; i<PKT_HDR_SIZE+rx_pkt_len; i++)
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
		}*/

		//schedule for TX -- 5ms for guarding period (open radio earlier)
		//my_slot=60; //don't use slot 1 and 62 if segment_period=1/8s, edge slots have timing issues
		if (my_slot != -1)
		{
			//PRINTF("Schedule for TX at Slot %d\n",my_slot);
			uint16_t SN_TX_time = SN_RX_start_time + (BS_period+TS_period * (my_slot-1))-32;//32 is for 16B payload, might need adjustment for other sizes

			rtimer_set(&SNTimer,SN_TX_time,0,TDMA_SN_send,NULL);
			//printf("%u,",SN_TX_time);
		}
		//printf("%05u,",SN_RX_start_time); //Beacon receive time
	}
	else if(SN_ID == 0) //BS
		/*-----------------BS CODE---------------*/
	{
		//set flag in pkt for TS occupancy SN_RX_start_time = packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP);
		//uint8_t current_TS = (uint8_t)((packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP)-BS_RX_start_time)/(TS_period));//*RTIMER_MS);

		char *rx_pkt = (char *)packetbuf_dataptr();
		uint16_t rx_pkt_len = rx_pkt[PKT_PAYLOAD_SIZE_INDEX];

		rtimer_clock_t relFrameTime =(rtimer_clock_t)((packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP)-radio_TX_time)%segment_period);
		uint8_t current_TS = (uint8_t)((relFrameTime-BS_period)/TS_period )+1;

		//printf(",%05u\n",packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP));
		if(node_list[current_TS-1] == FREE_SLOT_CONST) //collision -- ask the node to find a new available slot
		{
			node_list[current_TS-1] = rx_pkt[NODE_INDEX];
		}


		printf("Slot: %u, \n",current_TS);
		PRINTF("[Sensor: %d] [Slot: %d] [Seq: %d]\n",
				rx_pkt[NODE_INDEX],current_TS,rx_pkt[SEQ_INDEX]);

		PRINTF("Channel: %d;", cc2420_get_channel());
		PRINTF("RSSI: %d\n", cc2420_last_rssi-45);

		// callback to application layer

	}
	app_conn_input();

}
/*-----------------------------------------------*/
// turn on RDC layer
static int on(void)
{
	PRINTF("turn on RDC layer\n");
	if (SN_ID == 0) //BS sends packet
	{
		rtimer_set(&BSTimer,RTIMER_NOW()+segment_period,0,TDMA_BS_send,NULL);//segment_period*
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

	//check the if the number of time slot is large enough
	uint32_t min_segment_len = TS_period*total_slot_num + BS_period;
	if (min_segment_len > segment_period)
	{
		printf("min_segment_len > segment_period\n");
		assert(1);
	}


	//allocate node_list space
	if (SN_ID == 0)
	{
		node_list = (char *)malloc(total_slot_num);
		memset(node_list,FREE_SLOT_CONST,total_slot_num);
	}

	// allocate packet space
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
