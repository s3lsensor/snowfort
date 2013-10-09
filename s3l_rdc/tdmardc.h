/**
 * \file
 * 	A TDMA RDC implementation
 * \author
 * 	Yizheng Liao <yzliao@stanford.edu>
 */

#ifndef __TDMARDC_H__
#define __TDMARDC_H__

#include "net/mac/rdc.h"
#include "dev/radio.h"

extern const struct rdc_driver tdmardc_driver;

#define RTIMER_MS 33 // rtimer_second = 32768, closest integer estimate of ms. actually 1.0071 ms

// packet format -- removed when using Zigbee frame
static char pkt_hdr[] = {65,-120,-120,-51,-85,-1,-1, SN_ID, 0, 0};
//FCF: 65,-120
//Data Sequence number: -120
//Address information: -51 -85(CD AB in hex) -1 -1 (short address mode)
//Actually in the payload: SN_ID,0,0

#define PKT_HDR_SIZE 		10
#define NODE_INDEX   		7
#define SEQ_INDEX   		8
#define PKT_PAYLOAD_SIZE_INDEX    	9

#define FREE_SLOT_CONST 	0x7F
#define MAX_PKT_SIZE		127
#define MAX_PKT_PAYLOAD_SIZE	50		//should be 117, let's start from 50 right now

// time slot information -- default
#ifndef SEGMENT_PERIOD
#define SEGMENT_PERIOD	1100	//1100ms
#endif

#ifndef TS_PERIOD
#define TS_PERIOD 		100		//100ms
#endif

#ifndef BS_PERIOD
#define BS_PERIOD		100		//100ms
#endif

#ifndef TOTAL_TS
#define TOTAL_TS		10
#endif

#endif /* __TDMARDC_H__ */

