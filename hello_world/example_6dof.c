/*
* I2C sensor tsl2561 example
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


#include "sys/rtimer.h"
#include "dev/uart1.h"


#include "dev/6dof.h"

#define SAMPLING_FREQ 256 




/*---------------------------------------------------------------*/
PROCESS(null_app_process, "6dof test");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();

	printf("6dof test.\n");

	app_conn_open(0);


	int i;
	adxl345_union adx;
	itg3200_union itg;

	if (node_id != 0){
		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/SAMPLING_FREQ));
	}else{
		etimer_set(&rxtimer,CLOCK_SECOND/20);
	}

	if(node_id != 0){
		while(1){
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
			etimer_reset(&rxtimer);
			// intitiate each sensor before sample
			adxl345_enable();
			adx = adxl345_sample();
			itg3200_enable();
			itg = itg3200_sample(); 
			// print data
			printf("adxl data: x:%d, %d, y:%d, %d, z:%d, %d\n", 
				adx.x.h, adx.x.l, adx.y.h, adx.y.l, adx.z.h, adx.z.l);
			printf("itg data: x:%d, %d, y:%d, %d, z:%d, %d, tmp:%d, %d\n\n", 
				itg.x.h, itg.x.l, itg.y.h, itg.y.l, itg.z.h, itg.z.l, itg.temp.h, itg.temp.l);
			

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
