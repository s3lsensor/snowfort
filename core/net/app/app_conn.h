/*
 * app_conn.h
 *
 *  Created on: Aug 20, 2013
 *      Author: yzliao
 */
/**
 * app_conn.h
 * \file
 * 			Header file for application connector
 *  \author
 *  		Yizheng Liao (yzliao@stanford.edu)
 */


#ifndef APP_CONN_H_
#define APP_CONN_H_

/**
 * \brief	Callback structure for application callback
 */
struct app_callbacks
{
	/* Called when a packet has been received by the TDMA RDC layer. */
	void (*recv)(void);
};

/**
 * \brief	Set up/register the application callback connector
 */
void app_conn_open(const struct app_callbcks *u);

/**
 * \brief	Close the application callback connector
 */
void app_conn_close(void);

/*
 * \brief	API for RDC to call
 */
void app_conn_input(void);




#endif /* APP_CONN_H_ */
