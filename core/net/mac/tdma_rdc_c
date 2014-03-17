#include "net/mac/tdma_rdc.h"
#include "sys/rtimer.h"
#include "net/packetbuf.h"
#include "net/queuebuf.h"
#include "net/netstack.h"
#include <string.h>
#include "tic-toc.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif


#define RTIMER_MS (RTIMER_SECOND/1000.0)

static uint16_t beacon_period=500; // Beacon period in ms
static uint16_t slot_period=50; // Slot duration in ms
static uint16_t beacon_duration=50; //duration of Beacon Transmission
static uint8_t num_slots;
static uint8_t pkt_sz;
static uint8_t my_slot;


#define NODE_INDEX 7
#define SEQ_INDEX 8
#define PKT_SZ 9
#define PKT_HDR_SZ 9
static char pkt[] = {65, -120, -120, -51, -85, -1, -1, SN_ID, 0};

static struct rtimer beaconTimer;
static struct rtimer slotTimer;
static struct rtimer sleepModeTimer;


/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
}
char seq=0;
static void
simple_send()
{
  tic(RTIMER_NOW(), "send");
  // Set Timer for next expected Beacon RTIMER_MS must be before the slot_period
  //  uint16_t time =  RTIMER_NOW()+RTIMER_MS*(beacon_period-beacon_duration-(slot_period*SN_ID));
  uint16_t time =  RTIMER_NOW()+RTIMER_MS*(beacon_period-beacon_duration-(slot_period*my_slot));
  rtimer_set(&beaconTimer,time, 0, NETSTACK_RADIO.on, NULL);

  int ret;
  pkt[SEQ_INDEX] = seq++;
  if(NETSTACK_RADIO.on()){
    if(NETSTACK_RADIO.send(pkt, PKT_SZ) == RADIO_TX_OK) {
      ret = MAC_TX_OK;
    } else {
      ret =  MAC_TX_ERR;
    }
    NETSTACK_RADIO.off();
    printf("Sensor Sent: %d\n", pkt[SEQ_INDEX]);
  }
  else {
    // Error 
  }
  // Return ret
}
/*---------------------------------------------------------------------------*/
static void
send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *buf_list)
{
  // Not needed in TDMA RDC
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
  char *rx_data = (char *)packetbuf_dataptr();
  // Ignore if not a beacon
  if (rx_data[NODE_INDEX] != (char)255) return;

  tic(RTIMER_NOW(), "pkt_in");
  toc();
  tic(RTIMER_NOW(), "pkt_in");
  
  NETSTACK_RADIO.off();

  uint8_t free_slots = 0;
  uint8_t alloc = 0;

  my_slot = 0xff; //don't have a slot yet
  // count number of free slots
  // or slot already allocated to me
  int i=0;
  for (i=0; i<num_slots; i++) {
    alloc = rx_data[PKT_HDR_SZ+i];
    if (alloc == 0xff) {
      free_slots++;
    } else if (alloc == SN_ID) {
      my_slot = i;
    }
  }

  //I don't have a slot yet, select a random free slot
  uint8_t my_slot_index = RTIMER_NOW()%free_slots;
  if (my_slot == 0xff) {
    // not allocated yet
    // loop for all free slots
    for (i=0; i<num_slots; i++) {
      alloc = rx_data[PKT_HDR_SZ+i];
      if (alloc == 0xff && my_slot_index==0) {
	my_slot=i;
	//	printf("Selected a random slot: %d\n", my_slot);
	break;
      } else if (alloc == 0xff) {
	my_slot_index--;
      }
    }
  }

  // Set Timer for the timeslot - This must be scheduled before beacon
  //  uint16_t time = RTIMER_NOW()+RTIMER_MS*(beacon_duration-2+(SN_ID*slot_period));
  uint16_t time = RTIMER_NOW()+RTIMER_MS*(beacon_duration-2+(my_slot*slot_period));
  rtimer_set(&slotTimer, time, 0, simple_send, NULL);
  printf("Beacon: %d\n", rx_data[SEQ_INDEX]);
  print_tics();
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  printf("RDC just got ON \n");
  return NETSTACK_RADIO.on();
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  if(keep_radio_on) {
    return NETSTACK_RADIO.on();
  } else {
    return NETSTACK_RADIO.off();
  }
}
/*---------------------------------------------------------------------------*/
static unsigned short
channel_check_interval(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  num_slots = (beacon_period-beacon_duration)/slot_period ;
  pkt_sz = num_slots + PKT_HDR_SZ;
  on();
}
/*---------------------------------------------------------------------------*/
const struct rdc_driver tdmardc_driver = {
  "tdmardc",
  init,
  send_packet,
  send_list,
  packet_input,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
