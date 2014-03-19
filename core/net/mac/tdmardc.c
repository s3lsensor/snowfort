/**
 * \file
 * 	A TDMA RDC implementation
 * \author
 * 	Yizheng Liao <yzliao@stanford.edu>
 */

//#include "project-conf.h"
#include "node-id.h"
#include "net/mac/tdmardc.h"
#include "lib/assert.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "sys/rtimer.h"
#include "sys/timer.h"
#include "sys/etimer.h"
#include "clock.h"
#include "net/queuebuf.h"
#include "dev/cc2420.h"
#include "appconn/app_conn.h"
#include "net/mac/framer-tdma.h"
#include "frame802154.h"
#include <string.h>
#include <stdio.h>

#ifdef SF_FEATURE_SHELL_OPT
#include "remote-shell.h"
#endif



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
static const uint16_t total_slot_num = TOTAL_TS; // uint16_t to support > 256 slots
static volatile uint16_t sf_tdma_slot_num; //

//packet information
static uint8_t seq_num;

volatile rtimer_clock_t radio_TX_time;

#ifdef SF_MOTE_TYPE_AP
// BS global variable
static rtimer_clock_t BS_RX_start_time = 0;
//static uint8_t *node_list; //allocated, initialized in init()


//Timer -- BS
static struct rtimer BSTimer;
#endif /*SN_MOTE_TYPE_AP*/

#ifdef SF_MOTE_TYPE_SENSOR
// SN global variable
static rtimer_clock_t SN_RX_start_time = 0;
static uint16_t radioontime;
//static unit16_t radioofftime;

//Timer -- SN

uint8_t nMissedBeacons =0; // number of missed beacons - global
uint8_t rxData = 0; // received data indicator - global
static struct rtimer SNTimer;
static struct etimer sleeptimer; // timer for sleep state.
#define maxMissedBeacons 10 // beacons to miss before sleeping
#define sleep_time_s 300 // time to sleep after missing maxMissedBeacons beacons.

#endif

// RDC buffer
char tdma_rdc_buffer[MAX_PKT_PAYLOAD_SIZE] = {0};
volatile uint8_t tdma_rdc_buf_ptr = 0;
volatile uint8_t tdma_rdc_buf_send_ptr = 0;
volatile uint8_t tdma_rdc_buf_full_flg = 0;
volatile uint8_t tdma_rdc_buf_in_using_flg = 0;

// set slot number
void sf_tdma_set_slot_num(const uint16_t num)
{
  if (num  == 0 && num > TOTAL_TS)
    return;
  else
  {
    sf_tdma_slot_num = num;
  }
}

// get slot number
uint16_t sf_tdma_get_slot_num(void)
{
  return sf_tdma_slot_num;
}


// set_addr -- clean rime address and reset rime & cc2420 address
void sf_tdma_set_mac_addr(void)
{
  rimeaddr_t addr;
  uint8_t longaddr[8];
  uint16_t shortaddr;

  // reset rime address
#ifdef SF_MOTE_TYPE_AP
  memcpy(&addr,&rimeaddr_null,sizeof(rimeaddr_t));
#endif

#ifdef SF_MOTE_TYPE_SENSOR
  memset(&addr,0,sizeof(rimeaddr_t));
  addr.u8[0] = node_id & 0xff;
  addr.u8[1] = node_id >> 8;
#endif

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

#ifdef SF_MOTE_TYPE_AP
// TDMA_BS_send() -- called at a specific time
static void TDMA_BS_send(void)
{

  //printf("%05u,",RTIMER_NOW());//call time for BS_send

  uint8_t bkn_len = 16;
  uint8_t bkn_pkt[16]={0};
  bkn_pkt[14]=1;
  bkn_pkt[15]=2;

  // set timer for next BS send
  // right now, rtimer_timer does not consider drifting. For long time experiment, it may have problem
  rtimer_set(&BSTimer,RTIMER_TIME(&BSTimer)+segment_period,0,TDMA_BS_send,NULL);

  //update packet sequence number
  seq_num = seq_num + 1;

  if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_CMD) // has command to send
  {
    // Assume for BS, if the tdma_rdc_buf is not empty, then the payload should be command.
    // Should be changed if BS can send other types of data.
/*
    packetbuf_copyfrom((void *)&tdma_rdc_buffer[0],sizeof(uint8_t)*tdma_rdc_buf_ptr);
    packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_CMD);
    tdma_rdc_buf_full_flg = 0;
    tdma_rdc_buf_ptr = 0;
    tdma_rdc_buf_send_ptr = 0;
*/
    PRINTF("send command %s %d\n",tdma_rdc_buffer,packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE));
  }
  else
  {
    packetbuf_copyfrom((void *)&bkn_pkt,sizeof(uint8_t)*bkn_len);
    packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE, PACKETBUF_ATTR_PACKET_TYPE_TIMESTAMP);
  }


  BS_RX_start_time = radio_TX_time+BS_period;

  packetbuf_set_attr(PACKETBUF_ATTR_MAC_SEQNO,seq_num);


  uint8_t hdr_len = NETSTACK_FRAMER.create();

  // fail to create framer
  if(hdr_len < 0)
    return;

  //send packet -- pushed to radio layer
  if(NETSTACK_RADIO.send(packetbuf_hdrptr(),packetbuf_totlen()) != RADIO_TX_OK)
    printf("TDMA RDC: BS fails to send packet\n");
  else
    printf("TDMA RDC: BS sends %u\n",seq_num);
  packetbuf_clear(); // clear buffer so we don't get false commands later.


}
#endif /* SF_MOTE_TYPE_AP */

#ifdef SF_MOTE_TYPE_SENSOR


// TDMA_SN_send() -- called at a assigned time slot
static void TDMA_SN_send(void)
{
  //set timer for open RADIO -- for opening earlier 2 ms
  //uint16_t time = RTIMER_TIME(&SNTimer)+RTIMER_MS*(segment_period-BS_period-my_slot*TS_period);
  uint16_t callBkTime = RTIMER_NOW();
  radioontime = SN_RX_start_time+segment_period-GRD_PERIOD;//RTIMER_TIME(&SNTimer) + (total_slot_num-my_slot)*TS_period-GRD_PERIOD;//(segment_period-BS_period-(my_slot)*TS_period - GRD_PERIOD);
  // rtimer_set(&SNTimer,radioontime,0,NETSTACK_RADIO.on,NULL);
  rtimer_set(&SNTimer, radioontime, 0, TDMA_SN_listen, NULL);

  //update packet sequence number
  seq_num = seq_num + 1;

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


// Process for the radio to sleep after missing too many beacons.
PROCESS(SN_sleep_process, "SN Sleep Process");
PROCESS_THREAD(SN_sleep_process, ev, data){
  PROCESS_BEGIN();
  while(1){
    etimer_set(&sleeptimer,CLOCK_SECOND*sleep_time_s);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleeptimer));
    etimer_set(&sleeptimer, SEGMENT_PERIOD);
    NETSTACK_RADIO.on();
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sleeptimer));
    if(nMissedBeacons == 0) { //rediscovered base
      PROCESS_EXIT();
    } 
  }
  PROCESS_END();
}

// This function turns the radio off after a period of listening if no data has been received by the node. It then sets the timer to start listening again at a later time.
static void TDMA_SN_scheduleNextEvent(void) {
  if(rxData == 1) { // schedule transmission.
    if (sf_tdma_slot_num != -1){
      // printf("Schedule for TX at Slot %d\n",sf_tdma_slot_num);
      rtimer_clock_t SN_TX_time = SN_RX_start_time + (BS_period+TS_period * (sf_tdma_slot_num-1))-GRD_PERIOD+33;//20 might need to be changed later, do better calibration, increase to 33 if timing problems
      rtimer_set(&SNTimer,SN_TX_time,0,TDMA_SN_send,NULL);
    }
    rxData = 0;
    app_conn_input();
    return;
  } else { // no data, increment missed beacons and turn off radio.
    NETSTACK_RADIO.off();
    SN_RX_start_time = SN_RX_start_time + SEGMENT_PERIOD; //assumed new bkn
    nMissedBeacons++;
    printf("Missed beacon, total missed: %d\n", nMissedBeacons);
    if(nMissedBeacons >= maxMissedBeacons) { // sleep for longer time.
       printf("Missed %d beacons, going to sleep.\n", nMissedBeacons);
      process_start(&SN_sleep_process, NULL); // go to sleep.
    } else { //wake up again in one cycle.
      rtimer_clock_t next_bkn_time = SN_RX_start_time + SEGMENT_PERIOD - GRD_PERIOD;
      rtimer_set(&SNTimer,next_bkn_time,0,TDMA_SN_listen,NULL);
    }
  }
}

//Turns on the radio to listen for new data from the base and sets the timer to turn off the radio if no data is received.
static void TDMA_SN_listen(void) {
  clock_time_t listen_interval = BS_period*2 + SN_RX_start_time + SEGMENT_PERIOD - GRD_PERIOD; //assume no packets more than 2*base transmit period after expected time.
  rtimer_set(&SNTimer, listen_interval, 0, TDMA_SN_scheduleNextEvent, NULL);
  NETSTACK_RADIO.on(); //activate radio to listen for data.
}

    
#endif /*SF_MOTE_TYPE_SENSOR*/

/*-----------------------------------------------*/
// send packet
static void send(mac_callback_t sent_callback, void *ptr_callback)
{


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
  uint8_t incorrectDecode = 0;
  if(NETSTACK_FRAMER.parse() < 0){
    printf("Incorrect decode frame\n");
    incorrectDecode = 1;
  }

#ifdef SF_MOTE_TYPE_SENSOR
  
  /*-------------SN CODE----------------------*/
  //check if the packet is from BS
  if (!rimeaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_SENDER),&rimeaddr_null))
  {
    printf("Packet is not from base station, rejected!\n");
    return;
  }

  uint8_t *rx_pkt = (uint8_t *)packetbuf_dataptr();
  uint16_t rx_pkt_len = packetbuf_datalen();
 
 if(rx_pkt_len == 0 || incorrectDecode == 1) {
    rxData = 0;
    return;
  }
 
  //turn off radio -- save power
  if(NETSTACK_RADIO.off() != 1)
  {
    printf("TDMA RDC: SN fails to turn off radio");
  }

  SN_RX_start_time = packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP);
 
  /*--------from BS------------*/
  if (packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_CMD)
    { //Received command.
    nMissedBeacons = 0;
    rxData = 1;
    //skip this period for TX
    rtimer_clock_t next_bkn_time = SN_RX_start_time + SEGMENT_PERIOD - GRD_PERIOD;
    rtimer_set(&SNTimer, next_bkn_time, 0, TDMA_SN_listen, NULL);
#ifdef SF_FEATURE_SHELL_OPT
    char command_string[128];
    strncpy(command_string,rx_pkt,rx_pkt_len);
    command_string[rx_pkt_len] = (uint8_t)'\0';
    PRINTF("RX Command: %s %d\n",command_string,strlen(command_string));
    remote_shell_input();

    return;
#endif /* SF_FEATURE_SHELL_OPT */
  }
  else if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_DATA)
  {
    // schedule end of wait function, which will schedule transmit (prevents TDMA_SN_wait function from altering transmit timing).
    nMissedBeacons = 0;
    rxData = 1;
    rtimer_set(&SNTimer, clock_time(), 0, TDMA_SN_scheduleNextEvent, NULL);
    return;
  }
  
#endif /* SF_MOTE_TYPE_SENSOR */

#ifdef SF_MOTE_TYPE_AP
  /*-----------------BS CODE---------------*/

  rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
  uint8_t sent_sn_id = sent_sn_addr->u8[0];

  rtimer_clock_t relFrameTime =(rtimer_clock_t)((packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP)-radio_TX_time)%segment_period);
  uint16_t current_TS = (uint16_t)((relFrameTime-BS_period)/TS_period )+1;

  //set flag in pkt for TS occupancy
/*
  if(node_list[current_TS-1] == FREE_SLOT_CONST) //collision -- ask the node to find a new available slot
  {
    //node_list[current_TS] = rx_pkt[NODE_INDEX];
    node_list[current_TS-1] = sent_sn_id;
  }
*/

  PRINTF("[Sensor: %u] [Slot: %u] [Seq: %u]\n",
         sent_sn_id,current_TS,packetbuf_attr(PACKETBUF_ATTR_PACKET_ID));

  PRINTF("Channel: %d;", cc2420_get_channel());
  PRINTF("RSSI: %d\n", cc2420_last_rssi-45);

  if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_DATA)
  {
    // callback to application layer
    app_conn_input();
  }

#endif /*SF_MOTE_TYPE_AP */


}
/*-----------------------------------------------*/
// turn on RDC layer
static int on(void)
{
  PRINTF("turn on RDC layer\n");
#ifdef SF_MOTE_TYPE_AP
    rtimer_set(&BSTimer,RTIMER_NOW()+segment_period,0,TDMA_BS_send,NULL);
#endif
  return NETSTACK_RADIO.on();
}
/*-----------------------------------------------*/
// turn off RDC layer
static int off(int keep_radio_on)
{
  if (keep_radio_on)
    return NETSTACK_RADIO.on();
  else
  {
    PRINTF("turn off RDC layer\n");
    return NETSTACK_RADIO.off();
  }
}
/*-----------------------------------------------*/
// check channel -- do not need in TDMA implementation
static unsigned short channel_check_interval(void)
{
  return 0;
}
/*-----------------------------------------------*/
// Initialize RDC layer
static void init(void)
{
  //get node id
  node_id_restore();

  //set slot number
  sf_tdma_set_slot_num(node_id);


  //reset rime & radio address
  sf_tdma_set_mac_addr();

  //reset packet number
  seq_num = 0;

  //check the if the number of time slot is large enough
  uint32_t min_segment_len = TS_period*total_slot_num + BS_period;
  if (min_segment_len > segment_period)
  {
    printf("min_segment_len > segment_period\n");
    assert(1);
  }



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
