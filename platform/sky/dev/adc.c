/*
 * This file implements the adc functions and calls
 * corresponding architecture-specific functions
 * (implemented in /cpu/msp430/f1xxx for sky mote)
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#include "contiki.h"
#include "dev/adc.h"

static unsigned char adc_on_status = 0;

void adc_on(void)
{
	adc_on_status=1;
	adc_arch_on();
}

void adc_off(void)
{
	adc_on_status=0;
	adc_arch_off();
}

int adc_configure(unsigned char chan_num)
{
	return adc_arch_configure(chan_num);
}

unsigned char adc_status(void)
{
	return adc_on_status;
}

unsigned short adc_sample(adc_data_union *sampled_data)
{
	sampled_data->data = adc_arch_sample();
	//SWAP(sampled_data->reg.adc_h,sampled_data->reg.adc_l);

	return 1;
}
