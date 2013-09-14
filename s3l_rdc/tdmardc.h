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
#include "sys/rtimer.h"

extern const struct rdc_driver tdmardc_driver;

//#define RTIMER_MS 33 // rtimer_second = 32768, closest integer estimate of ms. actually 1.0071 ms
//#define RTIMER_MS (RTIMER_SECOND/1000.0)

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
#define MAX_PKT_PAYLOAD_SIZE	60		//should be 117, let's start from 50 right now

// time slot information -- default
#ifndef SEGMENT_PERIOD
#define SEGMENT_PERIOD	(RTIMER_SECOND/8) //993//1092	//equivalent to 1100 ms, 33*1092~=1.1*32768
#endif

#ifndef TOTAL_TS
#define TOTAL_TS		58
#endif

#ifndef TS_PERIOD
#define TS_PERIOD 		(SEGMENT_PERIOD/(TOTAL_TS+1.0))//410//1638//3277//99		//100ms
#endif

#ifndef BS_PERIOD
#define BS_PERIOD		(SEGMENT_PERIOD/(TOTAL_TS+1.0))//410//819//1638//3277//99		//100ms
#endif

#ifndef GRD_PERIOD
#define GRD_PERIOD		65 //ticks for 2 ms
#endif



#endif /* __TDMARDC_H__ */

