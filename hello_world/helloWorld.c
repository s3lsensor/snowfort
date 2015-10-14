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
//#include "i2c.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120
#define RESOLUTION 7


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
//APP Callback function
static void app_recv(void)
{
	//printf("Received from RDC\n");
	PROCESS_CONTEXT_BEGIN(&null_app_process);
	
	uint8_t *data = packetbuf_dataptr();
	uint8_t flag = 0;


	int i;
	rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
	uint8_t rx_sn_id = sent_sn_addr->u8[0];

	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	uint8_t payload_len = packetbuf_datalen();


//	printf("%u,%u,%u%c",rx_sn_id,pkt_seq,payload_len,'|');
//	for(i=0;i<payload_len;i++){
//		printf("%02x",data[i]);
//	}
//	printf("\n");

	app_output(data,rx_sn_id,pkt_seq,payload_len);

	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};


/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("Hello world Started\n");

	app_conn_open(&nullApp_callback);

	static uint16_t debug_buf[7] = {0};
	static struct etimer rxtimer;
	static char input_buf[MAX_PKT_PAYLOAD_SIZE] = {0};
	static uint16_t counter = 0;


	if (node_id != 0)
		//etimer_set(&rxtimer,(unsigned long)(SEGMENT_PERIOD));
		etimer_set( &rxtimer, (unsigned long)(CLOCK_SECOND/256));
	else
		etimer_set(&rxtimer,CLOCK_SECOND/256);

	//init_mpu6050();
	//uint8_t rv;
	//rv = read_(MPU_ADDRESS, 0x75, 0);
	//printf("%d \n", rv);

	if(node_id != 0)
	{

	  while(1)
	  {

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));

	    etimer_reset(&rxtimer);


	    int i = 0;
	    for(i = 0; i < 7; i++)
	    {
		    counter++;
		    debug_buf[i] = sin(counter);
	    }

/*
	    counter++;
	    debug_buf[0] = sin(counter) << 6;

	    counter++;
	    debug_buf[1] = sin(counter) << 6;

	    counter++;
	    debug_buf[2] = sin(counter) << 6;

	    counter++;
	    debug_buf[3] = sin(counter) << 7;
	    counter++;
	    debug_buf[4] = sin(counter) << 7;
	    counter++;
	    debug_buf[5] = sin(counter) << 7;

	    counter++;
	    debug_buf[6] = sin(counter) << 4;

*/
	    app_conn_send((uint8_t *)debug_buf,sizeof(int16_t)*7);

	  }
	}
	PROCESS_END();
}

