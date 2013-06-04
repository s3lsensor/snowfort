#include <stdio.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "sys/etimer.h"
#include "sys/log.h"

#define PKT_HDR_SIZE 9
#define NODE_INDEX   7
#define SEQ_INDEX   8

#define SIN_TAB_LEN 120
#define RESOLUTION 7

static const int8_t SIN_TAB[] =
{
  0,6,13,20,26,33,39,45,52,58,63,69,75,80,
 85,90,95,99,103,107,110,114,116,119,121,
 123,125,126,127,127,127,127,127,126,125,
 123,121,119,116,114,110,107,103,99,95,90,
 85,80,75,69,63,58,52,45,39,33,26,20,13,6,
 0,-6,-13,-20,-26,-33,-39,-45,-52,-58,-63,
 -69,-75,-80,-85,-90,-95,-99,-103,-107,-110,
 -114,-116,-119,-121,-123,-125,-126,-127,-127,
 -127,-127,-127,-126,-125,-123,-121,-119,-116,
 -114,-110,-107,-103,-99,-95,-90,-85,-80,-75,
 -69,-63,-58,-52,-45,-39,-33,-26,-20,-13,-6
};

static int8_t sinI(uint16_t angleMilli)
{
  uint16_t pos;
  pos = (uint16_t) ((SIN_TAB_LEN * (uint32_t) angleMilli)/1000);
  return SIN_TAB[pos%SIN_TAB_LEN];
}

static int8_t sin(uint16_t angleMilli)
{
  return SIN_TAB[angleMilli%SIN_TAB_LEN];
}

PROCESS(null_app_process, "Null App Process");
AUTOSTART_PROCESSES(&null_app_process);

PROCESS_THREAD(null_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("Null App Started\n");
  
  
  static int8_t debug_buf[10] = {0};
  static struct etimer rxtimer;
  static char input_buf[10] = {0};
  static uint16_t counter = 0;
  
  
  if (SN_ID != 0)
    etimer_set(&rxtimer,CLOCK_SECOND);
  else
    etimer_set(&rxtimer,CLOCK_SECOND/20);
  
  
  
  while(1)
  {
    
    if(SN_ID != 0)
    {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
      
      etimer_reset(&rxtimer);
      packetbuf_copyfrom(debug_buf,sizeof(int8_t)*10);
      NETSTACK_RDC.send(NULL,NULL);
      
      //printf("NULLAPP: %d %d %d\n", debug_buf[0],debug_buf[1],debug_buf[2]);
      int i = 0;
      for(i = 0; i < 10; i++)
      {
	counter++;
	debug_buf[i] = sin(counter);
      }

    }
    else if (SN_ID == 0)
    {
      PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
      etimer_reset(&rxtimer);
      
      char* data = packetbuf_dataptr();
      uint8_t flag = 0;
      
      int i;
      for(i = 0; i < 10; i++)
      {
	if(data[i+9] != input_buf[i])
	{
	  flag++;
	  break;
	}
      }
      
      if (flag > 0)
      {
	memcpy(input_buf,data+9,10);
	//log_message("Input",input_buf);
	int node_id = data[NODE_INDEX];
	int pkt_seq = data[SEQ_INDEX];
	data = data + 9;
	
	printf("%u,%d,%u,%d %d %d %d %d ",node_id,pkt_seq,0,data[0],data[1],data[2],data[3],data[4]);
	printf("%d %d %d %d %d\n",data[5],data[6],data[7],data[8],data[9]);
	//printf("Input: %s\n",input_buf);
      }
      

    }
  }

  
  
  PROCESS_END();
}
