#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

//include for shell
#include "shell.h"
#include "serial-shell.h"
#include "remote-shell.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120

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
	return SIN_TAB[angleMilli%SIN_TAB_LEN];
}

static uint16_t counter = 0;
static struct ctimer ct;
static struct etimer et;
static int8_t data_buf[6] = {0};
/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Sine Wave Process");
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

	uart1_writeb(rx_sn_id);
	uart1_writeb(pkt_seq);
	uart1_writeb(payload_len);

	if(node_id != 0)
	{
		printf("%u,%u,%u,",rx_sn_id,pkt_seq,payload_len);
	}
	else
	{
	
		uart1_writeb(rx_sn_id);
		uart1_writeb(pkt_seq);
		uart1_writeb(payload_len);
		uint8_t i = 0;
		for(i = 0; i < payload_len; i++)
		{
			uart1_writeb(data[i]);
		}
	
	//	printf("%u,%u,%u,",rx_sn_id,pkt_seq,payload_len);
	}

/*
	for(i = 0; i < payload_len; i++)
	{
		uart1_writeb(data[i]);
	}
*/
	uart1_writeb('\n');


	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};


/*---------------------------------------------------------------*/

static void sample_fun(void)
{
	ctimer_reset(&ct);

	uint8_t i;
	for(i = 0; i < 6; i++)
	{
		data_buf[i] = sinI(counter);
		counter++;
	}
	app_conn_send((uint8_t *)data_buf,6*sizeof(int8_t));
	
	//printf("%d %d %d %d %d %d\n",data_buf[0],data_buf[1],data_buf[2],data_buf[3],data_buf[4],data_buf[5]);
	//printf("%s %s\n",ct.p->name,ct.next->p->name);

}

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("Sine Wave Started\n");

	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();
	

	app_conn_open(&nullApp_callback);
/*
	if (node_id != 0)
		etimer_set(&et,16);
	else
		etimer_set(&et,200);
*/
	if (node_id != 0)
	{
		ctimer_set(&ct,10,sample_fun,(void*)NULL);
/*
		while(1)
		{
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			etimer_reset(&et);
			uint8_t i;
			for(i = 0; i < 6; i++)
			{
				data_buf[i] = sinI(counter);
				counter++;
			}
			app_conn_send((uint8_t *)data_buf,6*sizeof(int8_t));
		}
*/
	}
/*
	else
	{
		while(1)
		{
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
			etimer_reset(&et);
		}
		
	}
*/



/*
	int8_t data_buf[6] = {0};
	static struct etimer et;
	//static uint16_t counter = 0;
	uint8_t i;

	etimer_set(&et,(unsigned long)102);

	if (node_id != 0)
	{
		while(1)
		{
			//etimer_set(&et,(unsigned long)102);
			//printf("here 1\n");
			printf("1timer start %lu interval %lu process %s\n",et.timer.start,et.timer.interval,et.p->name);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

			printf("if expired %u\n",etimer_expired(&et));

			etimer_reset(&et);

			printf("2timer start %lu interval %lu process %s\n",et.timer.start,et.timer.interval,et.p->name);
			printf("next timer start %lu interval %lu process %s\n",et.next->timer.start,et.next->timer.interval,et.next->p->name);


			for(i = 0; i < 6; i++)
			{
				data_buf[i] = sinI(counter);
				counter++;
			}


			printf("here 3\n");
			app_conn_send((uint8_t *)data_buf,6*sizeof(int8_t));
			printf("here 4\n");
		}
	}
*/
	PROCESS_END();
}