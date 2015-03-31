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

#define I2C_SENSOR
#define SAMPLING_FREQ 256
#define SAMPLES_PER_FRAME (SAMPLING_FREQ/FRAMES_PER_SEC_INT)


/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Hello world Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
//APP Callback function
static void app_recv(void)
{
	PROCESS_CONTEXT_BEGIN(&null_app_process);

#ifdef SF_MOTE_TYPE_AP
	mpu_data_acc_gyro_union data[SAMPLES_PER_FRAME];

	// TX info
	uint8_t i;
	rimeaddr_t *sent_sn_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
	uint8_t rx_sn_id = sent_sn_addr->u8[0];

	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	//DATA_SIZE;
	uint8_t payload_len = packetbuf_datalen();

	if(payload_len != MPU_DATA_ACC_GYRO_SIZE*SAMPLES_PER_FRAME)
	{
		//printf("Ignore packet %u,%u %u\n",rx_sn_id,pkt_seq,payload_len);
		//return;
		memcpy((uint8_t*)data,packetbuf_dataptr(),MPU_DATA_ACC_GYRO_SIZE*SAMPLES_PER_FRAME);
	}
	else
	{
		memcpy((uint8_t*)data,packetbuf_dataptr(),MPU_DATA_ACC_GYRO_SIZE*SAMPLES_PER_FRAME);
	}

	for(i=0;i<SAMPLES_PER_FRAME;i++)
	{
		// MPU data
		mpu_data_acc_gyro_union samples = data[i];


		MPU_PRINT_BYTE(rx_sn_id);
		MPU_PRINT_BYTE(0);
		MPU_PRINT_BYTE(pkt_seq);
		MPU_PRINT_BYTE(0);
		print_mpu_sample_acc_gyro(&samples);
		MPU_PRINT_BYTE('\n');
	}

#endif

	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	static struct etimer rxtimer;

	PROCESS_BEGIN();

	app_conn_open(&nullApp_callback);


	uint8_t i;
	uint8_t temp;
	static uint8_t MPU_status = 0;
	static mpu_data_acc_gyro_union samples;

	if (node_id != 0)
	{
		MPU_status = 0;
		for(i = 0; i < 100 & (~MPU_status);i++)
		{
			MPU_status = mpu_enable();
		}

		if (MPU_status == 0)
			printf("MPU could not be enabled.\n");

		MPU_status = 0;
		for(i = 0; i < 100 & (~MPU_status);i++)
		{
			MPU_status = mpu_wakeup();
		}

		if (MPU_status == 0)
		{
			printf("MPU could not be awakened.\n");
		}
		etimer_set(&rxtimer, (unsigned long)(CLOCK_SECOND/SAMPLING_FREQ));

	}
	else
	{
		etimer_set(&rxtimer,CLOCK_SECOND/20);
	}


	if (node_id != 0)
	{
		while(1)
		{
			//MPU_PRINT_BYTE(54);
			PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));
			etimer_reset(&rxtimer);

			//MPU_PRINT_BYTE(55);
			
			MPU_status = mpu_sample_acc(&samples);
			if (MPU_status != 0)
			{
				//printf("%u,%u,%u\n",samples.data.x,samples.data.y,samples.data.z);
				//print_mpu_sample_acc_gyro(&samples);
				// MPU_PRINT_BYTE(samples.reg.x_h);
				// MPU_PRINT_BYTE(samples.reg.x_l);
				// MPU_PRINT_BYTE(samples.reg.y_h);
				// MPU_PRINT_BYTE(samples.reg.y_l);
				// MPU_PRINT_BYTE('\n');
				// read_mpu_reg(MPU6050_RA_INT_STATUS,&temp);
				// printf("%u",temp);
				app_conn_send((uint8_t *)(&samples),MPU_DATA_ACC_GYRO_SIZE);
			}
			else
				printf("Cannot sample data\n");
		}
	}

	PROCESS_END();
}
