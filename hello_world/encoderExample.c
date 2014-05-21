#include <stdio.h>
#include <string.h>
#include "contiki.h"
#include "net/packetbuf.h"
#include "net/mac/tdmardc.h" // for flags to sync with tdma 
#include "net/coder/huffman_code.h"
#include "sys/etimer.h"
#include "appconn/app_conn.h"
#include "node-id.h"

#include "app_util.h"

/*---------------------------------------------------------------*/
PROCESS(null_app_process, "Encoder Example Process");
AUTOSTART_PROCESSES(&null_app_process);

/*---------------------------------------------------------------*/
//APP Callback function
static void app_recv(void)
{
//	printf("Received from RDC\n");
	PROCESS_CONTEXT_BEGIN(&null_app_process);
	
	char* data = packetbuf_dataptr();
	//uint8_t flag = 0;

	rimeaddr_t *sent_sn_addr = (rimeaddr_t *)packetbuf_addr(PACKETBUF_ADDR_SENDER);
	int rx_sn_id = sent_sn_addr->u8[0];

	int pkt_seq = packetbuf_attr(PACKETBUF_ATTR_PACKET_ID);
	int payload_len = packetbuf_datalen();
	app_output(data,rx_sn_id,pkt_seq,payload_len);

	PROCESS_CONTEXT_END(&null_app_process);

}
static const struct app_callbacks nullApp_callback= {app_recv};

/*---------------------------------------------------------------*/
PROCESS_THREAD(null_app_process, ev, data)
{
	PROCESS_BEGIN();
	printf("Encoder Example Started\n");

	//app_conn_open(&nullApp_callback);

	if (node_id == 0)
	{
		// int8_t num = 0;
		// uint16_t encode_num = 0;
		// for(num = -128; num < 127; num++)
		// {
		// 	encode_num = huffman_encoder_8bit(num);
		// 	printf("1origin:%d, encode: 0x%x\n",num,encode_num);
		// }

		printf("---------------------------\n");
		int16_t num2 = 0;
		uint32_t encode_num2 = 0;
		for (num2 = 1000; num2 <= 3500; num2++)
		{
			encode_num2 = huffman_encoder_16bit(num2);
			printf("2origin:%d, encode: 0x%lx\n",num2,encode_num2);
		}
	}

	PROCESS_END();
}