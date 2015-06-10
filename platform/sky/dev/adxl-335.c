/*
 * This file defines the driver for adxl-335
 *
 * ---------------------------------------------------------
 *
 * Author: Yizheng Liao
 */

 #include "contiki.h"

#include <stdio.h>

#include "dev/adc.h"
#include "dev/adxl-335.h"
 #include "dev/adc-arch.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

int adxl_enable(void)
{
	// enable adc
	adc_on();

	ADC12CTL0 |= MSC;
	clock_delay(20000);

	ADC12CTL0 |= REFON; // 1.5V reference

	ADC12CTL1 |= CONSEQ_1; // sequence of channels
	clock_delay(20000);

	// configure adc channels
	ADC12MCTL0 = 0 + SREF_0;	//channel 0, x axis, A1
	ADC12MCTL1 = 1 + SREF_0;	//channel 1, y axis, A2
	ADC12MCTL2 = 2 + SREF_0; //channel 2, z axis, A3
	ADC12MCTL3 = INCH_11 + EOS + SREF_0; // channel 3, (AVcc-AVss)/2

	printf("%d,%d,%d,%d\n",ADC12MCTL0,ADC12MCTL1,ADC12MCTL2,ADC12MCTL3);
	return 1;
}

unsigned short adxl_sample(adxl_data_union *sampled_data)
{
	ADC12CTL0 |=ENC + ADC12SC;
	unsigned int i = 0;
	for(i = 0; (i < 1000) && (ADC12CTL1 & ADC12BUSY);i++);

	sampled_data->x.data = (unsigned int)ADC12MEM0;
	sampled_data->y.data = (unsigned int)ADC12MEM1;
	sampled_data->z.data = (unsigned int)ADC12MEM2;
	sampled_data->vref.data = (unsigned int)ADC12MEM3;

	return 1;
}