/*
* I2C sensor htu21d example
*/

#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

#include "app_util.h"
#include "dev/adc.h"
#include "dev/leds.h"
#include "dev/i2c.h"

#include "dev/htu21d.h"

#include "sys/rtimer.h"

#include "dev/uart1.h"


#define SAMPLING_FREQ 256


/*---------------------------------------------------------------*/
PROCESS(null_app_process, "htu21d test");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();


	printf("htu21d test.\n");

	app_conn_open(0);


	int i;
	htu21d_data rlt;

	if (node_id != 0){
		// initiate
		htu21d_enable();
		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/SAMPLING_FREQ));
	}else{
		etimer_set(&rxtimer,CLOCK_SECOND/20);
	}

	if(node_id != 0){
		while(1){
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
			etimer_reset(&rxtimer);
			// sample
			rlt = htu21d_sample_hum();
			printf("humidity data: h:%d, l:%d, crc:%d\n", 
				rlt.h, rlt.l, rlt.crc);
			rlt = htu21d_sample_tmp();
			printf("temperature data: h:%d, l:%d, crc:%d\n\n", 
				rlt.h, rlt.l, rlt.crc);
			

			// delay for examination
			i=0;
			while(i<1000){
				_NOP();
				i++;
			}

		}
	}


	PROCESS_END();

}
