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
static uint16_t slot_period=5; // Slot duration in ms
static uint16_t beacon_duration=30; //duration of Beacon Transmission

#define NODE_INDEX 7
#define SEQ_INDEX 8
#define PKT_SZ 9
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
  uint16_t time =  RTIMER_NOW()+RTIMER_MS*(beacon_period-beacon_duration-(slot_period*SN_ID));
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
    printf("Sensor Data Sent: %d\n", pkt[SEQ_INDEX]);
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

  // Set Timer for the timeslot - This must be scheduled before beacon
  uint16_t time = RTIMER_NOW()+RTIMER_MS*(beacon_duration-1.5+(SN_ID*slot_period));
  rtimer_set(&slotTimer, time, 0, simple_send, NULL);

  printf("Beacon Seq: %d \n", rx_data[SEQ_INDEX]);
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
