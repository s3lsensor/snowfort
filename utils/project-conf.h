#ifndef __PROJECT_CONF_H__
#define __PROJECT_CONF_H__

#define NETSTACK_CONF_RDC     tdmardc_driver

//TODO: change mac protocol to TDMA mac protocol -- yzliao
#define NETSTACK_CONF_MAC     nullmac_driver

#define NETSTACK_CONF_FRAMER  framer_tdma

// define sensor type
#if (SN_ID == 0)
#define SF_MOTE_TYPE_AP
#endif /* SN_ID == 0: Access point/Base station*/ 

#if (SN_ID > 0)
#define SF_MOTE_TYPE_SENSOR
#endif /* SN_ID >0: Sensor node*/

//define feature
//#define SF_FEATURE_SHELL_OPT

#endif /* __PROJECT_CONF_H__ */
