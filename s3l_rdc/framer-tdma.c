/*
 * 
 */

/**
 * \file
 * 		MAC framer for TDMA
 * \author
 * 		Yizheng Liao <yzliao@stanford.edu>
 */

#include "net/mac/framer-tdma.h"
#include "net/packetbuf.h"

#define DEBUG 0

#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINTADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7])
#else
#define PRINTF(...)
#define PRINTADDR(addr)
#endif

//Define FCF value
#define FRAME_TDMA_BEACONFRAME  (0x00)
#define FRAME_TDMA_DATAFRAME    (0x01)
#define FRAME_TDMA_CMDFRAME     (0x03)

#define FRAME_TDMA_BEACON_DST    (0xff)

struct tdma_hdr
{
  uint16_t frame_type;
  uint8_t  seq_no;
  uint16_t dst_node_id;
  uint16_t src_node_id;
  uint8_t  payload_len;
};

/*-------------------------------------------------------------*/
static int create(void)
{
  struct tdma_hdr *hdr;
  
  if(packetbuf_hdralloc(sizeof(struct tdma_hdr)))
  {
    hdr = packetbuf_hdrptr();
   
    //frame_type
    /*TODO: cmd frame*/
    if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_DATA) //data
        memcpy(hdr->frame_type,FRAME_TDMA_DATAFRAME,sizeof(FRAME_TDMA_DATAFRAME));
    else if(packetbuf_attr(PACKETBUF_ATTR_PACKET_TYPE) == PACKETBUF_ATTR_PACKET_TYPE_STREAM) //beacon
        memcpy(hdr->frame_type,FRAME_TDMA_BEACONFRAME,sizeof(FRAME_TDMA_BEACONFRAME));

    //seq_no
    memcpy(hdr->seq_no,packetbuf_attr(PACKETBUF_ATTR_MAC_SEQNO),sizeof(uint8_t));

    //dst_node_id
    if(&(hdr->frame_type) == FRAME_TDMA_BEACONFRAME)
        memcpy(hdr->dst_node_id,FRAME_TDMA_BEACON_DST,sizeof(FRAME_TDMA_BEACON_DST));
    else if(&(hdr->frame_type) == FRAME_TDMA_DATAFRAME)
        memcpy(hdr->dst_node_id,0x00,sizeof(0x00));    // data frame only sends to BS (node_id = 0)
    
    //src_node_id
    mempcy(hdr->src_node_id,SN_ID,sizeof(SN_ID));


    //payload_len
    memcpy(hdr->payload_len,packetbuf_datalen(),sizeof(uint8_t));

    //TODO:add checksum
    
    return sizeof(struct tdma_hdr);
  }
  PRINTF("TDMA Frame create: too large %d",sizeof(struct tdma_hdr));
  return FRAMER_FAILED;

}
/*-------------------------------------------------------------*/
static int parse(void)
{
    struct tdma_hdr *hdr;
    hdr = packetbuf_dataptr();
    if(packetbuf_hdrreduce(sizeof(struct tdma_hdr)))
    {
           //frame_type
           //TODO: cmd frame
           if(&(hdr->frame_type) == FRAME_TDMA_DATAFRAME)
              packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_DATA);
           else if(&(hdr->frame_type) == FRAME_TDMA_BEACONFRAME)
              packetbuf_set_attr(PACKETBUF_ATTR_PACKET_TYPE,PACKETBUF_ATTR_PACKET_TYPE_STREAM);

            // seq_no
            packetbuf_set_attr(PACKETBUF_ATTR_MAC_SEQNO,&(hdr->seq_no));

          //dst_node_id
          
          //src_node_id

          //payload_len  

            return sizeof(struct tdma_hdr);
    }
    return FRAMER_FAILED;
}
/*-------------------------------------------------------------*/
const struct framer framer_tdma = 
{
    create, parse
};
