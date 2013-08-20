/*
 * app_callback.h
 *
 *  Created on: Aug 19, 2013
 *      Author: yzliao
 */

#ifndef APP_CALLBACK_H_
#define APP_CALLBACK_H_

//Callback function
struct app_driver
{
	char *name;

	/* RDC callback */
	void (* rdc_callback)(void);
};


#endif /* APP_CALLBACK_H_ */
