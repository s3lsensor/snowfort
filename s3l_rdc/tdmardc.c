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
#include "net/mac/framer-tdma.h"
#include "frame802154.h"
#include <string.h>
#include <stdio.h>

#include "remote-shell.h"



#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*-----------------------------------------------*/
// Global Variables

// Scheduling
static const uint16_t segment_period = SEGMENT_PERIOD;   //one round time in ms
static const uint16_t TS_period = TS_PERIOD;         //one time-slot duration in ms
static const uint16_t BS_period = BS_PERIOD;         //BS broadcasts duration in ms

//slot information
static const uint8_t total_slot_num = TOTAL_TS; // calculated in init()
static int8_t my_slot; // set in SN_send

//packet information
//static uint8_t *pkt; //allocated in init()
//static const uint8_t pkt_size = PKT_HDR_SIZE + MAX_PKT_PAYLOAD_SIZE; //set in init()
static char seq_num = 0;

// BS global variable
static rtimer_clock_t BS_TX_start_time = 0;
static rtimer_clock_t BS_RX_start_time = 0;
static uint8_t *node_list; //allocated, initialized in init()


// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static uint16_t radioontime;
char tdma_rdc_buffer[MAX_PKT_PAYLOAD_SIZE] = {0};
volatile uint8_t tdma_rdc_buf_ptr = 0; //updated when send() called (RDC_send()) directly
volatile uint8_t tdma_rdc_buf_send_ptr = 0; //updated when send() called (RDC_send()) directly
volatile uint8_t tdma_rdc_buf_full_flg = 0; //updated when send() called RDC_send()) directly
volatile uint8_t tdma_rdc_buf_in_using_flg = 0;


//Timer -- BS
static struct rtimer BSTimer;

//Timer -- SN
static struct rtimer SNTimer;

// set_addr -- clean rime address and reset rime & cc2420 address
static void sf_tdma_set_mac_addr(void)
{
  rimeaddr_t addr;
  uint8_t longaddr[8];
  uint16_t shortaddr;

  // reset rime address
  memset(&addr,0,sizeof(rimeaddr_t));
  addr.u8[0] = SN_ID & 0xff;
  addr.u8[1] = SN_ID >> 8;
  rimeaddr_set_node_addr(&addr);
  printf("Rime started with address ");
  printf("%u.%u\n",rimeaddr_node_addr.u8[0],rimeaddr_node_addr.u8[1]);

  // reset CC2420 address
  shortaddr = (rimeaddr_node_addr.u8[0] << 8) + rimeaddr_node_addr.u8[1];
  memset(longaddr,0,sizeof(longaddr));
  rimeaddr_copy((rimeaddr_t *)&longaddr,&rimeaddr_node_addr);
  printf("MAC %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ",
             longaddr[0], longaddr[1], longaddr[2], longaddr[3],
             longaddr[4], longaddr[5], longaddr[6], longaddr[7]);
  cc2420_set_pan_addr(IEEE802154_PANID, shortaddr, longaddr);
}


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

	//update packet sequence number
	seq_num++;

	//pkt content
	//pkt[SEQ_INDEX] = seq_num++;
	//pkt[PKT_PAYLOAD_SIZE_INDEX] = total_slot_num;
	//memcpy(pkt+PKT_HDR_SIZE,node_list,total_slot_num);
	//memcpy(pkt,node_list,total_slot_num);

	//copy pkt to packetbuff
	//packetbuf_copyfrom((void *)&pkt[0],total_slot_num);

	if(tdma_rdc_buf_ptr != 0) // has command to send
	{
	  packetbuf_copyfrom((void *)&tdma_rdc_buffer[0],sizeof(uint8_t)*tdma_rdc_buf_ptr);
	  packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_CMD);
	  tdma_rdc_buf_full_flg = 0;
    tdma_rdc_buf_ptr = 0;
    tdma_rdc_buf_send_ptr = 0;
    PRINTF("send command %s\n",tdma_rdc_buffer);
	}
	else
	{
	  packetbuf_copyfrom((void *)&node_list[0],sizeof(uint8_t)*total_slot_num);
	  packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_DATA);
	}


	packetbuf_set_attr(PACKETBUF_ATTR_MAC_SEQNO,seq_num);
	uint8_t hdr_len = NETSTACK_FRAMER.create();


	//send packet -- pushed to radio layer
	if(NETSTACK_RADIO.send(packetbuf_hdrptr(),packetbuf_totlen()) != RADIO_TX_OK)
		printf("TDMA RDC: BS fails to send packet\n");
	else
		printf("TDMA RDC: BS sends %d\n",seq_num);
}

// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
	//tic(RTIMER_NOW(),"SN send");


	//set timer for open RADIO -- for opening earlier 2 ms
	//uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
	radioontime = RTIMER_TIME(&SNTimer) + RTIMER_MS*(segment_period-BS_period-(my_slot)*TS_period - 2);
	rtimer_set(&SNTimer,radioontime,0,NETSTACK_RADIO.on,NULL);

	//pkt[SEQ_INDEX] = seq_num++;

	//update packet sequence number
	seq_num++;

	//wait if the tdma_rdc_buffer is accessing by other functions
	while(tdma_rdc_buf_in_using_flg);

	// lock tdma_rdc_buffer and preventing access from other functions.
  tdma_rdc_buf_in_using_flg = 1;


	if (tdma_rdc_buf_full_flg == 0)
	{
		packetbuf_copyfrom((void *)&tdma_rdc_buffer[0],sizeof(uint8_t)*tdma_rdc_buf_ptr);
		packetbuf_set_datalen(tdma_rdc_buf_ptr);
	}
	else
	{
		uint8_t temp_len = MAX_PKT_PAYLOAD_SIZE - tdma_rdc_buf_send_ptr;
		memcpy(packetbuf_dataptr(),tdma_rdc_buffer+tdma_rdc_buf_send_ptr,sizeof(uint8_t)*temp_len);
		memcpy(packetbuf_dataptr()+temp_len,tdma_rdc_buffer,sizeof(uint8_t)*tdma_rdc_buf_send_ptr);
		packetbuf_set_datalen(MAX_PKT_PAYLOAD_SIZE);
	}


	// send packet -- pushed to radio layer
	if(NETSTACK_RADIO.on())
	{
	  packetbuf_set_attr(PACKETBUF_ATTR_MAC_SEQNO,seq_num);
	  uint8_t hdr_len = NETSTACK_FRAMER.create();


	  int i = 0;
    uint8_t *hdr_ptr = packetbuf_hdrptr();
    for(i = 0; i < hdr_len; i++)
    {
      PRINTF("%u,",hdr_ptr[i]);
    }
    PRINTF("\n");


	  if(NETSTACK_RADIO.send(packetbuf_hdrptr(),packetbuf_totlen()) != RADIO_TX_OK)
		{
			printf("TDMA RDC: SN fails to send packet\n");
		}
		else
		{
			PRINTF("TDMA RDC: SN sends %d\n",seq_num);
		}
	  tdma_rdc_buf_full_flg = 0;
	  tdma_rdc_buf_ptr = 0;
	  tdma_rdc_buf_send_ptr = 0;
	}
	else
	{
		printf("TDMA RDC: SN fails to open radio\n");
	}
	// turn off radio
	NETSTACK_RADIO.off();

	// release tdma_rdc_buffer
  tdma_rdc_buf_in_using_flg = 0;
}

/*-----------------------------------------------*/
// send packet
static void send(mac_callback_t sent_callback, void *ptr_callback)
{

/*
  uint8_t data_len = packetbuf_datalen();

	uint8_t *ptr;
	ptr = (uint8_t *)packetbuf_dataptr();

	if((data_len+tdma_rdc_buf_ptr) <= MAX_PKT_PAYLOAD_SIZE)
	{
		memcpy(tdma_rdc_buffer+tdma_rdc_buf_ptr,ptr,data_len*sizeof(uint8_t));
		tdma_rdc_buf_ptr = tdma_rdc_buf_ptr + data_len;
	}
	else
	{
		uint8_t temp_len = MAX_PKT_PAYLOAD_SIZE-tdma_rdc_buf_ptr;
		memcpy(tdma_rdc_buffer+tdma_rdc_buf_ptr,ptr,temp_len*sizeof(uint8_t));
		tdma_rdc_buf_full_flg = 1;
		tdma_rdc_buf_ptr = 0;
		memcpy(tdma_rdc_buffer+tdma_rdc_buf_ptr,ptr+temp_len,(data_len-temp_len)*sizeof(uint8_t));
		tdma_rdc_buf_ptr = tdma_rdc_buf_ptr + data_len - temp_len;

	}

	if(tdma_rdc_buf_full_flg == 1)
	  tdma_rdc_buf_send_ptr = tdma_rdc_buf_ptr;

*/

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

  if(NETSTACK_FRAMER.parse() < 0)
    printf("Incorrect decode frame\n");

  uint8_t *rx_pkt = (uint8_t *)packetbuf_dataptr();
//  printf("RX:%u %u %u %u %u ",rx_pkt[0],rx_pkt[1],rx_pkt[2],rx_pkt[3],rx_pkt[4]);
//  printf("%u %u %u %u %u\n",rx_pkt[5],rx_pkt[6],rx_pkt[7],rx_pkt[8],rx_pkt[9]);
//	uint16_t rx_pkt_len = rx_pkt[PKT_PAYLOAD_SIZE_INDEX];
  uint16_t rx_pkt_len = packetbuf_datalen();

	/*-------------SN CODE----------------------*/
	if (SN_ID != 0) // sensor node -- decide timeslot & schedule for TX
	{
		SN_RX_start_time = RTIMER_NOW();

		//check if the packet is from BS
		if (!rimeaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_SENDER),&rimeaddr_null))
		{
		  return;
		}

		/*--------from BS------------*/

		//turn off radio -- save power
		if(NETSTACK_RADIO.off() != 1)
		{
			printf("TDMA RDC: SN fails to turn off radio");
		}

		if (packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_CMD)
    {
      remote_command_event_message = process_alloc_event();
      char command_string[128];
      strncpy(command_string,rx_pkt,rx_pkt_len);
      command_string[rx_pkt_len] = (uint8_t)'\0';
      PRINTF("RX Command: %s %d\n",command_string,strlen(command_string));

      process_post(&remote_shell_process,remote_command_event_message,command_string);

      return;
    }

		//first, check if BS assigns a slot
		unsigned char i = 0;
		char free_slot = 0;
		my_slot = -1;
		for(i = 0; i < rx_pkt_len; i++)
		{
			//print payload
			//PRINTF("%d ",rx_pkt[i]);
			if(SN_ID == rx_pkt[i])
			{
				my_slot = i;
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
			for(i = 0; i<rx_pkt_len; i++)
			{
				if(rx_pkt[i] == FREE_SLOT_CONST)
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

		//schedule for TX -- 2ms for guarding period (open radio earlier)

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
    rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
    uint8_t sent_sn_id = sent_sn_addr->u8[0];

	  //set flag in pkt for TS occupancy
		uint8_t current_TS = (RTIMER_NOW()-BS_RX_start_time)/(TS_period*RTIMER_MS);
		if(node_list[current_TS] == FREE_SLOT_CONST) //collision -- ask the node to find a new available slot
		{
			//node_list[current_TS] = rx_pkt[NODE_INDEX];
		  node_list[current_TS] = sent_sn_id;
		}



		PRINTF("[Sensor: %u] [Slot: %u] [Seq: %u]\n",
				sent_sn_id,current_TS,packetbuf_attr(PACKETBUF_ATTR_PACKET_ID));

		PRINTF("Channel: %d;", cc2420_get_channel());
		PRINTF("RSSI: %d\n", cc2420_last_rssi-45);

		if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_DATA)
		{
		  // callback to application layer
		  app_conn_input();
		}
		else if (packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_CMD)
		{
		  remote_command_event_message = process_alloc_event();
		  char command_string[128];
		  strncpy(rx_pkt,command_string,rx_pkt_len);
		  command_string[rx_pkt_len] = (uint8_t)'\0';

		  process_post(&remote_shell_process,remote_command_event_message,command_string);
		}
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

	//reset rime & radio address
  sf_tdma_set_mac_addr();

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
		node_list = (uint8_t *)malloc(total_slot_num);
		memset(node_list,FREE_SLOT_CONST,total_slot_num);
	}

	// allocate packet space
/*
	pkt = ( char *)malloc(pkt_size+1);
	memcpy(pkt,pkt_hdr,PKT_HDR_SIZE);
	if (SN_ID == 0)
		memset(pkt+PKT_HDR_SIZE,FREE_SLOT_CONST,total_slot_num); //set free slot
	else
		memset(pkt+PKT_HDR_SIZE,(uint8_t)0x7F,total_slot_num); //free payload for SN
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
