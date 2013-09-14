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
void app_output(const uint8_t * data, const int node_id, const int pkt_seq, const int payload_len)
{
	printf("%d,%d,%d%c",node_id,pkt_seq,payload_len,'|');
	int i;
	for(i = 0; i < payload_len; i++)
	{
		printf(",%.2x",data[i]);
	}
	printf("\n");
}

