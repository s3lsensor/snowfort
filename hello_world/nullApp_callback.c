/*

 * nullApp_callback.c
 *
 *  Created on: Aug 19, 2013
 *      Author: yzliao
 */
#include "nullApp_callback.h"

void rdc_callback(void)
{
	printf("Callback by rdc\n");
}

/***********************/
const struct app_driver nullApp_driver
{
	"nullApp",
	rdc_callback,
};

