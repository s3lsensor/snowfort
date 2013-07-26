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

// Define FCF Value
#define FRAME_TDMA_BEACONFRAME (0x00)
#define FRAME_TDMA_DATAFRAME 	(0x01)
#define FRAME_TDMA_CMDFRAME	(0x03)

#define FRAME_TDMA_BEACON_DST	(0xff)

int check_rx_finished(void);

struct tdma_hdr
{
  uint16_t frame_type;
  uint8_t  seq_num;
  uint8_t  dst_node_id;
  uint8_t  src_node_id;
  uint8_t  payload_len;
};



#endif /* __TDMARDC_H__ */

