/**
 * \file
 * 	A RDC implementation used to listen to the channel
 * \author
 * 	Yizheng Liao <yzliao@stanford.edu>
 */

#include "net/mac/quiterdc.h"
#include "net/packetbuf.h"
#include "net/netstack.h"
#include <string.h>
#include <stdio.h>
#include "tic-toc.h"


/*--------------------------------------------*/
// send packet -- do not need
static void send(mac_callback_t sent_callback, void *ptr)
{
}


/*--------------------------------------------*/
// send packet list -- do not need
static void send_list(mac_callback_t sent_callback, void *ptr, struct rdc_buf_list *list)
{
}

/*--------------------------------------------*/
// receives packet
static void input(void)
{
    printf("receiving data\n");
    
    uint8_t *rx_pkt = (uint8_t *)packetbuf_dataptr();
    uint16_t data_len = packetbuf_datalen();
    printf("data length %d\n",data_len);
/*    
    uint16_t i = 0;
    for(i = 0; i < 10; i++)
    {
      printf("%d ",rx_pkt[i]);
    }
    printf("\n");
*/
}

/*--------------------------------------------*/
// turn on RDC layer
static int on(void)
{
    printf("turn on RDC layer\n");
    
    if(NETSTACK_RADIO.on() == 1)
      printf("turn on radio\n");
    
    return 1;
}

/*--------------------------------------------*/
// turn off RDC layer
static int off(int keep_radio_on)
{
    if (keep_radio_on)
	return NETSTACK_RADIO.on();
    else
	return NETSTACK_RADIO.off();
}


/*--------------------------------------------*/
// check channel -- do not need here
static unsigned short channel_check_interval(void)
{
    return 0;
}

/*--------------------------------------------*/
// initalize RDC layer
static void init(void)
{
    printf("Init RDC layer\n");
    on();
}

/*--------------------------------------------*/
const struct rdc_driver quiterdc_driver =
{
    "quiterdc",
    init,
    send,
    send_list,
    input,
    on,
    off,
    channel_check_interval,
};
/*--------------------------------------------*/