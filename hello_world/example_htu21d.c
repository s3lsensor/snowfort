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

#define RESOLUTION 7

#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ MPU_SAMPLING_FREQ
#define SAMPLES_PER_FRAME MPU_SAMPLES_PER_FRAME

static uint32_t counterxx = 0;

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "htu21d test");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();


	printf("htu21d test.\n");

#ifdef SF_FEATURE_SHELL_OPT
	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();
#endif

	app_conn_open(0);



#ifdef ADC_SENSOR

	uint8_t i;
	static uint8_t sample_num = 0; //increments from 0 to samples_per_frame-1


	if (node_id != 0){
		soundDet_enable();
		etimer_set( &rxtimer, (unsigned long)(CLOCK_SECOND/(ADC_SAMPLING_FREQ)));
	}
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	if(node_id != 0)
	{

	  while(1)
	  {

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
	    etimer_reset(&rxtimer);

	    //samples[sample_num]=ml8511_sample();
	    sample_num++;
	    printf("%d read\n", soundDet_sample_audio());
	    if(sample_num == ADC_SAMPLES_PER_FRAME){
	    	sample_num=0;

	    	tdma_rdc_buf_ptr = 0;
	    	tdma_rdc_buf_send_ptr = 0;
	    	tdma_rdc_buf_full_flg = 0;
	    }

	  }
	}
#endif


#ifdef I2C_SENSOR
	int i;
	static uint8_t sample_count = 0;
	htu21d_data rlt;

	if (node_id != 0){
		// initiate
		htu21d_enable();
		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/MPU_SAMPLING_FREQ));
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

#endif

	PROCESS_END();

}
