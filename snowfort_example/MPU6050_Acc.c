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
#include "dev/mpu-6050.h"


#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


//global veriable
static struct ctimer ct;
static mpu_data_acc_gyro_union sample_data;
/*---------------------------------------------------------------*/
PROCESS(null_app_process, "I2C Acc Process");
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
	uart1_writeb(rx_sn_id);
	uart1_writeb(pkt_seq);
	uart1_writeb(payload_len);
*/
	if(node_id != 0)
	{
		printf("%u,%u,%u\n",rx_sn_id,pkt_seq,payload_len);
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
	uint8_t MPU_status = 0;
	MPU_status = mpu_sample_acc(&sample_data);
	if (MPU_status != 0)
	{
		printf("%d,%d,%d\n",sample_data.data.x,sample_data.data.y,sample_data.data.z);
		app_conn_send((uint8_t*)&sample_data,MPU_DATA_ACC_GYRO_SIZE);
	}
	else
	{
		printf("Cannot sample data\n");
	}
}

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("MPU6050 ACC Started\n");

	static uint8_t MPU_status2 = 0;
	uint8_t i;

	app_conn_open(&nullApp_callback);

	if (node_id != 0)
	{
		MPU_status2 = 0;
		for(i = 0; i < 100 &(~MPU_status2);i++)
		{
			MPU_status2 = mpu_enable();
		}

		if(MPU_status2 == 0)
		{
			printf("MPU could not be enabled\n");
		}

		MPU_status2 = 0;
		for(i = 0; i < 100 &(~MPU_status2);i++)
		{
			MPU_status2 = mpu_wakeup();
		}

		if(MPU_status2 == 0)
		{
			printf("MPU could not be awakened\n");
		}

		ctimer_set(&ct,16,sample_fun,(void*)NULL);
	}

	PROCESS_END();
}