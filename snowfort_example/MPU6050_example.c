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


static mpu_data_union sample_data;
static mpu_data_acc_gyro_union data_sample;
static mpu_data_tp_union tp_data_sample;

static uint8_t packet_counter = 0;
static uint8_t has_reset = 0;

static uint8_t MPU_status = 0;
/*---------------------------------------------------------------*/
PROCESS(null_app_process, "MPU 6050 Process");
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

		uint8_t i = 0;

		if(print_MPU != 0)
		{
			for(i = 0; i < (payload_len); i++)
			{
				//printf("%d,",((data[2*i+1]<<8)|data[2*i]));
				putchar(data[i]);
			}
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

	MPU_status = mpu_sample_all(&sample_data);



	if (MPU_status != 0)
	{

		PRINTF("%d,%d,%d,%d,%d,%d,%d\n",sample_data.data.accel_x,sample_data.data.accel_y,sample_data.data.accel_z,sample_data.data.tp,sample_data.data.gyro_x,sample_data.data.gyro_y,sample_data.data.gyro_z);

		// sending acceleration data
		mpu_get_acc(&sample_data,&data_sample);
		app_conn_send((uint8_t*)&data_sample,MPU_DATA_ACC_GYRO_SIZE);


		// sending gyro data
		mpu_get_gyro(&sample_data,&data_sample);
		app_conn_send((uint8_t*)&data_sample,MPU_DATA_ACC_GYRO_SIZE);


		// sending temperature data
		//mpu_get_tp(&sample_data,&tp_data_sample);
		//app_conn_send((uint8_t*)&tp_data_sample,MPU_DATA_TP_SIZE);


	}
	else
	{
		printf("Cannot sample data\n");
	}
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
	printf("MPU6050 Started\n");

#ifdef SF_FEATURE_SHELL_OPT
	serial_shell_init();
	remote_shell_init();
	shell_reboot_init();
	shell_blink_init();
	shell_sky_init();

#endif

	uint8_t i;

	app_conn_open(&nullApp_callback);

	if (node_id > 0)
	{
		MPU_status = 0;
		for(i = 0; i < 100 &(~MPU_status);i++)
		{
			MPU_status = mpu_enable();
		}

		if(MPU_status == 0)
		{
			printf("MPU could not be enabled\n");
		}

		MPU_status = 0;
		for(i = 0; i < 100 &(~MPU_status);i++)
		{
			MPU_status = mpu_wakeup();
		}

		if(MPU_status == 0)
		{
			printf("MPU could not be awakened\n");
		}

		/* configurate MPU6050 sensor */
		uint8_t MPU_config = 0;

		// disable sleep model
		read_mpu_reg(MPU_RA_PWR_MGMT1,&MPU_config);
		MPU_config = MPU_config & ~BV(6); // set bit 6 to 0
		write_mpu_reg(MPU_RA_PWR_MGMT1,MPU_config);
#if DEBUG
		read_mpu_reg(MPU_RA_PWR_MGMT1,&MPU_config);
		PRINTF("power management 1: %u\n",MPU_config);
#endif
		// disable cycle
		read_mpu_reg(MPU_RA_PWR_MGMT1,&MPU_config);
		MPU_config = MPU_config & ~BV(5); // set bit 5 to 0
		write_mpu_reg(MPU_RA_PWR_MGMT1,MPU_config);
#if DEBUG
		read_mpu_reg(MPU_RA_PWR_MGMT1,&MPU_config);
		PRINTF("power management 1: %u\n",MPU_config);
#endif

		// gyro range: -/+ 250 degree/sec
		read_mpu_reg(MPU_GYRO_CONFIG,&MPU_config);
		MPU_config = MPU_config & ~BV(3); // set bit 3 to zero
		write_mpu_reg(MPU_GYRO_CONFIG,MPU_config);
#if DEBUG
		read_mpu_reg(MPU_GYRO_CONFIG,&MPU_config);
		PRINTF("Gyro config: %u\n",MPU_config);
#endif

		// accelerometer range: -/+ 2g
		read_mpu_reg(MPU_ACCEL_CONFIG,&MPU_config);
		MPU_config = MPU_config & ~BV(3); // set bit 3 to zero -/+ 2g
		//MPU_config = MPU_config | BV(4); // set bit 4 to one -/+ 8g
		write_mpu_reg(MPU_ACCEL_CONFIG,MPU_config);
		read_mpu_reg(MPU_ACCEL_CONFIG,&MPU_config);
		printf("Acceleromter config: %u\n",MPU_config);

		// LPF: cut-off 21Hz for accel and 20Hz for gyro; DLPF_CFG = 4
		read_mpu_reg(MPU_CONFIG,&MPU_config);
		MPU_config = MPU_config | BV(2); // set bit 2 to 1, DLPF_CFG = 4
		write_mpu_reg(MPU_CONFIG,MPU_config);
#if DEBUG
		read_mpu_reg(MPU_CONFIG,&MPU_config);
		PRINTF("MPU 6050 config: %u\n",MPU_config);
#endif

		// sampling rate 1kHz
		write_mpu_reg(MPU_SMPLRT_DIV,0);
#if DEBUG
		read_mpu_reg(MPU_SMPLRT_DIV,&MPU_config);
		PRINTF("sample divider: %u\n",MPU_config);
#endif

		// start sampling
		ctimer_set(&ct,SAMPLE_RATE,sample_fun,(void*)NULL);
		ctimer_set(&reset_timer,SAMPLE_RATE*50,reset_sample_timer,(void*)NULL);
	}
	else
	{
		print_MPU = 1;
	}

	PROCESS_END();
}
