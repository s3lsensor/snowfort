#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Hello world Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process,ev,data)
{
	PROCESS_BEGIN();
	printf("Hello world\n");
	printf("This node has id %u\n",node_id);

	uint8_t i;
	for(i = 0; i < 256; i++)
	{
		printf("Hello World %d\n",i);
	}

	PROCESS_END();
}
