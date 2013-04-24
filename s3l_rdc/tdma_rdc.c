#include "net/mac/tdma_rdc.h"
#include "sys/rtimer.h"
#include "net/packetbuf.h"
#include "net/queuebuf.h"
#include "net/netstack.h"
#include <string.h>
#include "tic-toc.h"


#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif

#define RTIMER_MS (RTIMER_SECOND/1000.0)

/*-----------------------------------------------------------*/
static void send(mac_callback_t sent, void *ptr)
{
}

/*-----------------------------------------------------------*/
static void send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *buf_list)
{
}

/*-----------------------------------------------------------*/
static void input(void)
{
}

/*-----------------------------------------------------------*/
static int on(void)
{
	printf("RDC just got on \n");
	return NETSTACK_RADIO.on();
}


/*-----------------------------------------------------------*/
static int off(int keep_radio_on)
{
	if (keep_radio_on)
	{
		return NETSTACK_RADIO.on();
	}
	else
	{
		return NETSTACK_RADIO.off();
	}
}

/*-----------------------------------------------------------*/
static unsigned short
channel_check_interval(void)
{
	return 0;
}

/*-----------------------------------------------------------*/
static void init(void)
{
	//entrace point of rdc drive
	printf("Node ID %d\n",SN_ID);
	on();	
}





/*-----------------------------------------------------------*/
const struct rdc_driver tdma_rdc_driver =
{
	//extend from rdc.h
	"tdma_rdc",	// name
	init,		// initialize the RDC drive	
	send,		// send a packet from the Rime buffer
	send_list,	// send a packet list
	input,		// callback from getting notified of incoming packet
	on,		// turn the radio on
	off,		// turn the radio off
	channel_check_interval,	// returns the channel check interval, expressed in clock_time_t ticks
};
