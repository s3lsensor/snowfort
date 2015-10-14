/*
 * framer-tdma.c
 *
 *  Created on: Sep 25, 2013
 *      Author: yzliao
 */

/**
 * \file
 *         TDMA MAC framer for IEEE 802.15.4
 * \author
 *         Yizheng Liao <yzliao@stanford.edu>
 */

#include "net/mac/framer-tdma.h"
#include "net/mac/frame802154.h"
#include "net/packetbuf.h"
#include <string.h> /* for memset */

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7])
#else
#define PRINTF(...)
#define PRINTADDR(addr)
#endif
/*---------------------------------------------------------------------------*/
static int
is_broadcast_addr(uint8_t mode, uint8_t *addr)
{
  int i = mode == FRAME802154_SHORTADDRMODE ? 2 : 8;
  while(i-- > 0) {
    if(addr[i] != 0xff) {
      return 0;
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static int create(void)
{
	frame802154_t params;
	uint8_t len; //hdr length

	// Initialization
	memset(&params,0,sizeof(frame802154_t));

	/* Build the FCF */
	//params.fcf.frame_type = FRAME802154_DATAFRAME;
	switch (packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE))
	{
	  case PACKETBUF_ATTR_PACKET_TYPE_DATA: //data frame
	    params.fcf.frame_type = FRAME802154_DATAFRAME;
	    break;

	  case PACKETBUF_ATTR_PACKET_TYPE_CMD: // command frame & command ack frame
	    params.fcf.frame_type = FRAME802154_CMDFRAME;
	    break;

	  default:
	    params.fcf.frame_type = FRAME802154_DATAFRAME;
	    break;
	}

	params.fcf.security_enabled = 0;
	params.fcf.frame_pending = 0;	//do not support data request right now
	params.fcf.ack_required = 0;
	params.fcf.panid_compression = 0;

	/* Insert IEEE 802.15.4 (2003) version bit */
	params.fcf.frame_version = FRAME802154_IEEE802154_2003;

	/* Set the packet sequence number. */
	params.seq = packetbuf_attr(PACKETBUF_ATTR_MAC_SEQNO);

	/* Complete the addressing fields. */
	params.fcf.src_addr_mode = FRAME802154_SHORTADDRMODE;
	params.fcf.dest_addr_mode = FRAME802154_SHORTADDRMODE;

	/*
	 * If he dest address is NULL, then it is broadcast frame (beacon)
	 */
	if(rimeaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER),&rimeaddr_null))
	{
		params.dest_addr[0] = 0xFF;
		params.dest_addr[1] = 0xFF;
	}
	else
	{
		//this is not broadcast frame (data frame to BS)
		rimeaddr_copy((rimeaddr_t *)&params.dest_addr,
				packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
	}

	/* Set up the source address */
	rimeaddr_copy((rimeaddr_t *)&params.src_addr,&rimeaddr_node_addr);

	/* Set the PAN ID*/
	params.src_pid = IEEE802154_PANID;
	params.dest_pid = IEEE802154_PANID;

	/* Set payload */
	params.payload = packetbuf_dataptr();
	params.payload_len = packetbuf_datalen();

	len = frame802154_hdrlen(&params);

	if(packetbuf_hdralloc(len))
	{
		frame802154_create(&params,packetbuf_hdrptr(),len);
/*
		PRINTF("15.4-OUT: %2X", params.fcf.frame_type);
		PRINTADDR(params.dest_addr);
		PRINTF("%u %u (%u)\n", len, packetbuf_datalen(), packetbuf_totlen());
*/
		return len;
	}
	else
	{
		PRINTF("15.4-OUT: too large header: %u\n", len);
		return FRAMER_FAILED;
	}


}

/*---------------------------------------------------------------------------*/
static int parse(void)
{
	frame802154_t frame;
	int len;
	len = packetbuf_datalen();
	int8_t f1 = frame802154_parse(packetbuf_dataptr(),len,&frame);
	int8_t f2 = packetbuf_hdrreduce(len-frame.payload_len);
	if(f1 && f2)
	{
		if(frame.fcf.dest_addr_mode)
		{
			if(frame.dest_pid != IEEE802154_PANID &&
					frame.dest_pid != FRAME802154_BROADCASTPANDID)
			{
				//reject frame -- not for me
				PRINTF("15.4: for another pan %u\n", frame.dest_pid);
				return FRAMER_FAILED;
			}
			if(!is_broadcast_addr(frame.fcf.dest_addr_mode,frame.dest_addr))
			{
				packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (rimeaddr_t *)&frame.dest_addr);
			}
		}
		packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (rimeaddr_t *)&frame.src_addr);
		packetbuf_set_attr(PACKETBUF_ATTR_PACKET_ID, frame.seq);

		switch (frame.fcf.frame_type)
		{
		  case  FRAME802154_DATAFRAME:
		    packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_DATA);
		    break;

		  case  FRAME802154_CMDFRAME:
		    packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_CMD);
        	break;
		}
/*
		PRINTF("15.4-IN: %2X", frame.fcf.frame_type);
		PRINTADDR(packetbuf_addr(PACKETBUF_ADDR_SENDER));
		PRINTADDR(packetbuf_addr(PACKETBUF_ADDR_RECEIVER));
		PRINTF("%u (%u)\n", packetbuf_datalen(), len);
*/

		return len - frame.payload_len;
	}
	else
	{
		PRINTF("DECODE %d header reduce %d data len %u\n",f1,f2,len);
		return FRAMER_FAILED;
	}
	
}

/*---------------------------------------------------------------------------*/
const struct framer framer_tdma = {
  create, parse
};

