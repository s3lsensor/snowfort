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
static uint16_t rt_slot_duration; //slot_duration in RTIMER units
static uint16_t beacon_duration=50; //duration of Beacon transmission
static uint16_t rt_ref;
static uint8_t num_slots;
static uint8_t pkt_sz;

#define NODE_INDEX 7
#define SEQ_INDEX 8
#define PKT_HDR_SZ 9
static char pkt_hdr[] = {65, -120, -120, -51, -85, -1, -1, 255, 0};
static char* pkt;

static struct rtimer beaconTimer;

/*---------------------------------------------------------------------------*/
char seq=0;
static void
send_beacon(void)
{
  int i=0;
  tic(RTIMER_NOW(), "beacon");
  toc();
  tic(RTIMER_NOW(), "beacon");
  rt_ref = RTIMER_NOW()+RTIMER_MS*beacon_duration;
  rtimer_set(&beaconTimer, RTIMER_TIME(&beaconTimer)+RTIMER_MS*beacon_period,0, send_beacon, NULL);
  pkt[SEQ_INDEX] = seq++;
  if(NETSTACK_RADIO.send(pkt, pkt_sz) == RADIO_TX_OK ) {
    // no error
  } else {
    PRINTF("tdma_rdc.c:In send_beacon - BEACON NOT SENT\n");
  }
  // reseting the allocation
  memset(&(pkt[PKT_HDR_SZ]), 0xff, num_slots);
  // print tics for previous beacon
  print_tics(); 
  printf("Beacon Sent: %d\n", pkt[SEQ_INDEX]);
}

/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
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
  tic(RTIMER_NOW(), "pkt_in");
  char *rx_data = (char *)packetbuf_dataptr();
  char slot_id = (RTIMER_NOW()-rt_ref)/rt_slot_duration;
  char sensor_id = rx_data[NODE_INDEX];
  pkt[PKT_HDR_SZ+slot_id] = sensor_id; 
  printf("[Sensor: %d]  [Slot: %d]  [Seq: %d] \n", rx_data[NODE_INDEX], slot_id, rx_data[SEQ_INDEX]);
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  printf("RDC just got ON \n");
  rt_slot_duration=RTIMER_MS*slot_period;
  rtimer_set(&beaconTimer, RTIMER_NOW()+RTIMER_MS*beacon_period,0, send_beacon, NULL);
  return NETSTACK_RADIO.on();
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  printf("RDC: off\n");
  rtimer_set(&beaconTimer, RTIMER_NOW(),0, NULL, NULL);

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
  // initialize the allocation
  num_slots = (beacon_period-beacon_duration)/slot_period ;
  pkt_sz = num_slots + PKT_HDR_SZ;
  pkt = malloc(pkt_sz);
  // free all slots
  memset(&(pkt[PKT_HDR_SZ]), 0xff, num_slots);
  memcpy(pkt, pkt_hdr, PKT_HDR_SZ);
  int i=0;
  for (i=0; i<pkt_sz; i++) { printf(" i:%d pkt[i]:%d ", i, pkt[i]); }
  printf("\n");
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
