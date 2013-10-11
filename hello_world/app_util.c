/*
 * app_util.c
 *
 *  Created on: Aug 3, 2013
 *      Author: yzliao
 */

#include "app_util.h"

/*
 * Function: app_output
 * This function prints the payload data to stdout. The printing format is consistent
 * with the read_usb.py script. It is recommended for using this function to print
 * output instead of using the self-developed function.
 */
void app_output(const uint8_t * data, const int sn_id, const int pkt_seq, const int payload_len)
{
	/*printf("%u,%d,%u",node_id,pkt_seq,0);
	int i;
	for(i = 0; i < payload_len; i++)
	{
		printf(",%d",data[i]);
	}
	printf("\n");*/

	int i;
	printf("%u,%u,%u%c",sn_id,pkt_seq,payload_len,'|');
	for(i=0;i<payload_len;i++){
		printf("%02x",data[i]);
	}
	printf("\n");
}

