/*
 * app_util.c
 *
 *  Created on: Aug 3, 2013
 *      Author: yzliao
 */

#include "app_util.h"
#include <stdio.h>
//#include "platform-conf.h"


/*
 * Function: app_output
 * This function prints the payload data to stdout. The printing format is consistent
 * with the read_usb.py script. It is recommended for using this function to print
 * output instead of using the self-developed function.
 */
void app_output(const uint8_t * data, const uint8_t node_id, const uint8_t pkt_seq, const uint8_t payload_len)
{
  int i;

  printf("%u,%u,%u",node_id,pkt_seq,payload_len);
  int16_t * data_int16 = (int16_t *)data;

	for(i = 0; i < (payload_len/2); i++)
	{
	  printf("%d,",data_int16[i]);
	}
	printf("\n");
}

