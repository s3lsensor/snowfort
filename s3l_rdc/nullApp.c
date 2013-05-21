#include <stdio.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "sys/etimer.h"

PROCESS(null_app_process, "Null App Process");
AUTOSTART_PROCESSES(&null_app_process);

PROCESS_THREAD(null_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("Null App Started\n");
  
  
  static uint8_t debug_buf[3] = {0,1,2};
  static struct etimer rxtimer;
  etimer_set(&rxtimer,CLOCK_SECOND);
  
  while(1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
    if(SN_ID != 0)
    {
      etimer_reset(&rxtimer);
      packetbuf_copyfrom(debug_buf,sizeof(uint8_t)*3);
      NETSTACK_RDC.send(NULL,NULL);
      
      //printf("NULLAPP: %d %d %d\n", debug_buf[0],debug_buf[1],debug_buf[2]);
      
      debug_buf[0] += 3;
      debug_buf[1] += 3;
      debug_buf[2] += 3;
    }
    else
      etimer_stop(&rxtimer);
  }

  
  
  PROCESS_END();
}
