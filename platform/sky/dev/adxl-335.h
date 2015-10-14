/*
 * This file defines the driver for ADXL-335
 *
 * ---------------------------------------------------------
 *
 * Author: Yizheng Liao
 */
#ifndef ADXL_335_H
#define ADXL_335_H

#include "dev/adc.h"

typedef struct 
{
	/* data */
	adc_data_union x;
	adc_data_union y;
	adc_data_union z;
	adc_data_union vref;
}adxl_data_union;

#define ADXL_DATA_SIZE (sizeof(adxl_data_union)/sizeof(uint8_t))

int adxl_enable(void);
unsigned short adxl_sample(adxl_data_union *sampled_data);

#endif /*ADXL_335_H*/