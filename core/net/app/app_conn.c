/*
 * app_conn.c
 *
 *  Created on: Aug 20, 2013
 *      Author: yzliao
 */
/*
 * \file
 * 		Application callback connector
 * \author
 * 		Yizheng Liao <yzliao@stanford.edu>
 */

#include "app_conn.h"
#include "stdlib.h"

#define DEBUG 1
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

/* Local Variable */
// Local variable used to hold application callback pointer
static struct app_callbacks* app_callback_API;

/*---------------------------------------------------------------------------*/
void app_conn_open(const struct app_callbcks *u)
{
	app_callback_API = u;
}

/*---------------------------------------------------------------------------*/
void app_conn_close(void)
{
	app_callback_API = Null;
}

/*---------------------------------------------------------------------------*/
void app_conn_input(void)
{
	app_callback_API->recv();
}
