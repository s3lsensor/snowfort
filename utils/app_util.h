/*
 * app_util.h
 *
 *  Created on: Aug 3, 2013
 *      Author: yzliao
 */

#ifndef APP_UTIL_H_
#define APP_UTIL_H_

//#include <stdio.h>
#include "contiki.h"

void app_output(const uint8_t * data, const uint8_t node_id, const uint8_t pkt_seq, const uint8_t payload_len);

#endif /* APP_UTIL_H_ */
