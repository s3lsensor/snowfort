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


// headers for I2C sensors
//#include "dev/tsl2561.h"
//#include "dev/ms5803.h"
#include "dev/6dof.h"
//#include "dev/htu21d.h"

// headers for ADC sensors
//#include "dev/ml8511.h"
//#include "dev/adxl337.h"
//#include "dev/soundDet.h"


#include "sys/rtimer.h"

#include "dev/uart1.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120
#define RESOLUTION 7


#define ADC_SAMPLING_FREQ 64 //use power of 2 in Hz (tested 1, 2, 4...32)
#define ADC_SAMPLES_PER_FRAME (ADC_SAMPLING_FREQ/FRAMES_PER_SEC)

#define MPU_SAMPLING_FREQ 256 //tested 1, 2, and 4
#define MPU_SAMPLES_PER_FRAME (MPU_SAMPLING_FREQ/FRAMES_PER_SEC_INT)

#define I2C_SENSOR
//#define ADC_SENSOR

#ifdef I2C_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ MPU_SAMPLING_FREQ
#define SAMPLES_PER_FRAME MPU_SAMPLES_PER_FRAME
#endif /*I2C_SENSOR*/

#ifdef ADC_SENSOR
#define DATA_SIZE sizeof(uint16_t)/sizeof(uint8_t);
#define SAMPLING_FREQ ADC_SAMPLES_PER_FRAME
#define SAMPLES_PER_FRAME ADC_SAMPLES_PER_FRAME
#endif /*ADC_SENSOR*/

#ifndef DATA_SIZE
#define DATA_SIZE 1 //size of uint8_t (1 bit)
#endif /*DATA_SIZE*/


static uint32_t counterxx = 0;


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

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Hello world Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{

	static struct etimer rxtimer;

	PROCESS_BEGIN();


	printf("Hello world Started.\n");

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
	int16_t* coeff;
	adxl345_union adx;
	itg3200_union itg;

	if (node_id != 0){
		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/MPU_SAMPLING_FREQ));
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

#endif

	PROCESS_END();

}
