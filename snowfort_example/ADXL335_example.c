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
#include "dev/adxl-335.h"
#include "dev/leds.h"

//include for shell
#ifdef SF_FEATURE_SHELL_OPT
#include "shell.h"
#include "serial-shell.h"
#include "remote-shell.h"
#endif


#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#define SAMPLE_RATE 20 //ticks.1024 ticks per sec & fs = 50Hz

//global veriable
static struct ctimer ct,reset_timer;

static adxl_data_union sample;

static uint8_t packet_counter = 0;
static uint8_t has_reset = 0;

static uint8_t ADC_status = 0;
/*---------------------------------------------------------------*/
PROCESS(null_app_process, "ADXL 335 Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
//APP Callback function
static void app_recv(void)
{
	PROCESS_CONTEXT_BEGIN(&null_app_process);

	uint8_t *data = packetbuf_dataptr();

	rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);

	uint8_t rx_sn_id = sent_sn_addr->u8[0];
	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	uint8_t payload_len = packetbuf_datalen();

/*
	If this node is base station, then print out received messages.

	The current print function prints the output in ASCII code because
	the execution time can be reduced significantly. If the execution
	time is not a constraint, then printf should work as well.

	Please see readUSB_MPU6050_BS.py
*/

	if(node_id == 0)
	{
		//start byte
		putchar(126);
		putchar(126);

		// packet information

		packet_counter = packet_counter + 1;
		packet_counter = ((packet_counter == 10) ? 11 : packet_counter);

		 putchar(rx_sn_id);
		 putchar(packet_counter);
		 putchar(pkt_seq);
		 putchar(payload_len);
		//putchar(0);

		// //printf("%u",rx_sn_id);
		uint8_t i = 0;

		for(i = 0; i < (payload_len); i++)
		{
			//printf("%d,",((data[2*i+1]<<8)|data[2*i]));
			putchar(data[i]);
		}



	}



	uart1_writeb('\n');
	uart1_writeb('\n');


	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};

/*---------------------------------------------------------------*/
static void sample_fun(void)
{

	ctimer_reset(&ct);
	ctimer_restart(&reset_timer);


	ADC_status = adxl_sample(&sample);

	if (ADC_status == 1)
		app_conn_send((uint8_t*)&sample,ADXL_DATA_SIZE);

	PRINTF("%u,%u,%u,%u\n",sample.x.data,sample.y.data,sample.z.data,sample.vref.data);


}

static void reset_sample_timer(void)
{
	//ctimer_set(&ct,SAMPLE_RATE,sample_fun,(void*)NULL);
	ctimer_reset(&ct);
	ctimer_restart(&reset_timer);
	has_reset = has_reset + 1;
	printf("reset sample fun ctimer %u\n",has_reset);
}

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("ADXL335 Started\n");

#ifdef SF_FEATURE_SHELL_OPT
	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();

#endif

	//static uint8_t MPU_status3 = 0;

	app_conn_open(&nullApp_callback);

	if (node_id > 0)
	{
		ADC_status = 0;

		adxl_enable();

		// start sampling
		ctimer_set(&ct,SAMPLE_RATE,sample_fun,(void*)NULL);
		ctimer_set(&reset_timer,SAMPLE_RATE*50,reset_sample_timer,(void*)NULL);
	}


	PROCESS_END();
}
