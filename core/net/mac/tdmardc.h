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

#include "stdint.h"




//#define FREE_SLOT_CONST 	0x7F
#define MAX_PKT_SIZE		127
#define MAX_PKT_PAYLOAD_SIZE	116		//should be 117, let's start from 50 right now

// time slot information -- default
/*
#ifndef SLOT_NUM
#define SLOT_NUM SN_ID //test for edge slots (1,61,62) for 1/s seg. period
#endif
*/
#ifndef FRAMES_PER_SEC
#define FRAMES_PER_SEC 6.0
#define FRAMES_PER_SEC_INT 6
#endif

#ifndef SEGMENT_PERIOD
#define SEGMENT_PERIOD	(RTIMER_SECOND/FRAMES_PER_SEC) //993//1092	//equivalent to 1100 ms, 33*1092~=1.1*32768
#endif

#ifndef TOTAL_TS
#define TOTAL_TS		6
#endif

#ifndef BKN_SLOTS
#define BKN_SLOTS 		1
#endif

#ifndef TS_PERIOD
#define TS_PERIOD 		(SEGMENT_PERIOD/(TOTAL_TS+BKN_SLOTS))//410//1638//3277//99		//100ms
#endif

#ifndef BS_PERIOD
#define BS_PERIOD		(BKN_SLOTS*TS_PERIOD)//410//819//1638//3277//99		//100ms
#endif

#ifndef GRD_PERIOD
#define GRD_PERIOD		65 //ticks for 2 ms
#endif

#ifndef MAX_LISTEN_PERIOD
#define MAX_LISTEN_PERIOD	(30*CLOCK_SECOND)
#endif

#ifndef MAX_SLEEP_PERIOD
#define MAX_SLEEP_PERIOD	(60*CLOCK_SECOND)
#endif

/* data structure for transferring data*/
// extern uint8_t tdma_rdc_buffer[MAX_PKT_PAYLOAD_SIZE];
// extern volatile uint8_t tdma_rdc_buf_ptr; //updated when send() called (RDC_send()) directly
// extern volatile uint8_t tdma_rdc_buf_send_ptr; //updated when send() called (RDC_send()) directly
// extern volatile uint8_t tdma_rdc_buf_full_flg; //updated when send() called RDC_send()) directly
// extern volatile uint8_t tdma_rdc_buf_in_using_flg;

/* tdma rdc driver */
extern const struct rdc_driver tdmardc_driver;

/* extern function */
extern void sf_tdma_set_slot_num(const uint16_t num);
extern uint16_t sf_tdma_get_slot_num(void);
extern void sf_tdma_set_mac_addr(void);

#endif /* __TDMARDC_H__ */

