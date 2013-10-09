#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "sys/etimer.h"
#include "appconn/app_conn.h"

#include "app_util.h"
//#include "i2c.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define SIN_TAB_LEN 120
#define RESOLUTION 7

#define MPU_ADDRESS 0xD0
/*
static int16_t accx, accy, accz;
static int16_t gyrx, gyry, gyrz;

static uint8_t measurevector[14];

static void init_mpu6050(){
	write_(MPU_ADDRESS, 0x6B, 0x01); //clear sleep bit, set clock to 0x01 (x-gyro)
	write_(MPU_ADDRESS, 0x1B, 0x00); //fs_250 for gyro
	write_(MPU_ADDRESS, 0x1C, 0x00); //fs_2 for accel
}

static void measure_mpu(){
//	uint8_t measurevector[14];
	read_multibyte(MPU_ADDRESS, 0x3B, 14, measurevector);

	accx = 0;
	accx |= measurevector[0];
	accx = (accx<<8) | measurevector[1];
	accy = 0;
	accy |= measurevector[2];
	accy = (accy<<8) | measurevector[3];
	accz = 0;
	accz |= measurevector[4];
	accz = (accz<<8) | measurevector[5];

	gyrx = 0;
	gyrx |= measurevector[8];
	gyrx = (gyrx<<8) | measurevector[9];
	gyry = 0;
	gyry |= measurevector[10];
	gyry = (gyry<<8) | measurevector[11];
	gyrz = 0;
	gyrz |= measurevector[12];
	gyrz = (gyrz<<8) | measurevector[13];

}
 */

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
PROCESS(null_app_process, "Null App Process");
//PROCESS(sensor_sampling_process, "Sensor Sampling Process");
//AUTOSTART_PROCESSES(&null_app_process, &sensor_sampling_process);
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
	rimeaddr_t *rime_node_addr = packetbuf_addr(PACKETBUF_ADDR_SENDER);
	uint8_t node_id = rime_node_addr->u8[0];
	uint8_t pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	uint8_t payload_len = packetbuf_datalen();

	PRINTF("%u,%u,%u,%c",node_id,pkt_seq,payload_len,'|');

	if(payload_len < 45){
		for(i=0;i<payload_len;i++)
		{
			PRINTF("%.2x",data[i]);
		}
	}
	else {

	}
	PRINTF("\n");



	//app_output(data+PKT_HDR_SIZE,node_id,pkt_seq,payload_len);


	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};


/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("Null App Started\n");

	app_conn_open(&nullApp_callback);

	static uint8_t debug_buf[10] = {0};
	static struct etimer rxtimer;
	static char input_buf[MAX_PKT_PAYLOAD_SIZE] = {0};
	static uint16_t counter = 0;


	if (SN_ID != 0)
		//etimer_set(&rxtimer,(unsigned long)(SEGMENT_PERIOD));
		etimer_set( &rxtimer, (unsigned long)(CLOCK_SECOND/(FRAMES_PER_SEC)));
	else
		etimer_set(&rxtimer,CLOCK_SECOND/20);

	//init_mpu6050();
	//uint8_t rv;
	//rv = read_(MPU_ADDRESS, 0x75, 0);
	//printf("%d \n", rv);

	if(SN_ID != 0)
	{

	  while(1)
	  {

	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&rxtimer));

	    etimer_reset(&rxtimer);

	    //measure_mpu();
	    //printf("Accel value: %d\tY value: %d\tZ value: %d\n",accx,accy,accz);

	    //      packetbuf_copyfrom(debug_buf,sizeof(int8_t)*10);
	    //      NETSTACK_RDC.send(NULL,NULL);

	    //printf("NULLAPP: %d %d %d\n", debug_buf[0],debug_buf[1],debug_buf[2]);
	    int i = 0;
	    for(i = 0; i < 10; i++)
	    {
		    counter++;
		    debug_buf[i] = sin(counter)+127;
	    }

	    app_conn_send(debug_buf,sizeof(int8_t)*10);


	  }
	}
	PROCESS_END();
}

/*
PROCESS_THREAD(sensor_sampling_process, ev, data)
{
  PROCESS_BEGIN();
  if (SN_ID != 0){
	printf("Sensor Sampling begun\n");

 	 static struct etimer sensetimer;
  	  etimer_set(&sensetimer,CLOCK_SECOND);
	  init_mpu6050();  
	  uint8_t rv;
	  rv = read_(MPU_ADDRESS, 0x75, 0);
	  printf("%d \n", rv);


	  while(1)
	  {

//	    if(SN_ID != 0)
//    {
  	    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sensetimer));

  	    etimer_reset(&sensetimer);

  	    measure_mpu();
	    #if DEBUG
  	    printf("Accel value: %d\tY value: %d\tZ value: %d\n",accx,accy,accz);
	    #endif

	    packetbuf_copyfrom(measurevector,sizeof(int8_t)*10);
	    NETSTACK_RDC.send(NULL,NULL);

  	  }
  	}
  PROCESS_END();
}
 */
