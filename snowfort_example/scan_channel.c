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

static struct ctimer ct;

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Scan Channel Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
static void scan_channel(void)
{
	uint i = 0;
	int rx_power = 0;
	for(i = 11; i <= 26; i++)
	{
		cc2420_set_channel(i);
		rx_power = cc2420_rssi()-45;
		printf("Channel %u: %d dBm\n",i,rx_power);
	}
	printf("\n");
	ctimer_reset(&ct);
}

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process,ev,data)
{
	PROCESS_BEGIN();
	printf("Scan Channel\n");
	printf("This node has id %u\n",node_id);

	ctimer_set(&ct,CLOCK_SECOND*5,scan_channel,(void*)NULL);

	PROCESS_END();
}