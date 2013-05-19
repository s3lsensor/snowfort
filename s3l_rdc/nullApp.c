#include <stdio.h>
#include "contiki.h"
#include "net/packetbuf.h"
//#include "sys/timer.h"
//#include "contiki-conf.h"
#include "net/netstack.h"

#define DEBUG 1

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7])
#else
#define PRINTF(...)
#define PRINTADDR(addr)
#endif

PROCESS(null_app_process, "Null App Process");
AUTOSTART_PROCESSES(&null_app_process);

PROCESS_THREAD(null_app_process, ev, data)
{
  PROCESS_BEGIN();
  printf("Null App Started\n");
  PRINTADDR(&rimeaddr_node_addr);
/*  
  struct timer rxtimer;
  timer_set(&rxtimer,CLOCK_SECOND);

  uint8_t debug_buf[3] = {0,1,2};

  while(1)
  {
      if(timer_expired(&rxtimer) && SN_ID != 0)
      {
	  packetbuf_copyfrom(debug_buf,sizeof(uint8_t)*3);
	  //PRINTF("Call here\n");
	  NETSTACK_RDC.send(NULL,NULL);
	  timer_reset(&rxtimer);
	  debug_buf[0] += 3;
	  debug_buf[1] += 3;
	  
	  debug_buf[2] += 3;
	  
      }
  }

*/
  PROCESS_END();
}
