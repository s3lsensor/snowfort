/*
 * app_util.c
 *
 *  Created on: Aug 3, 2013
 *      Author: yzliao
 */

#include "app_util.h"
#include <stdio.h>

#ifdef MSP430DEF_H
int putint(int c)
{
  uart1_writeb((char)c);
  return c;
}

void putchar(char c)
{
  uart1_writeb(c);
}
#endif

/*
 * Function: app_output
 * This function prints the payload data to stdout. The printing format is consistent
 * with the read_usb.py script. It is recommended for using this function to print
 * output instead of using the self-developed function.
 */
void app_output(const char * data, const int node_id, const int pkt_seq, const int payload_len)
{
	printf("%u,%d",node_id,pkt_seq);
	int i;
	for(i = 0; i < payload_len; i++)
	{
#ifdef MSP430DEF_H
	  putchar(',');
	  putint(data[i]);
#else
	  printf(",%d",data[i]);
#endif
	}
	printf("\n");
}

