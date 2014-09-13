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
#if (SN_ID == 10)
#error "SN_ID cannot be 10" //ascii 10 is line break
#endif
#define SF_MOTE_TYPE_SENSOR
#endif /* SN_ID >0: Sensor node*/

//define feature
#define SF_FEATURE_SHELL_OPT

//redefine time accuracy
#ifdef CLOCK_CONF_SECOND
#undef CLOCK_CONF_SECOND
#define CLOCK_CONF_SECOND 1024UL
#else
#define CLOCK_CONF_SECOND 1024UL
#endif

#endif /* __PROJECT_CONF_H__ */
