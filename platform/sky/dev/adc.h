/*
 * This file defines the adc functions which are architecture
 * specific and implemented in /cpu/msp430/f1xxx
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */

#ifndef ADC_H
#define ADC_H

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

typedef struct 
{
	/* data */
	int8_t adc_h;
	int8_t adc_l;

}adc_lh;

typedef union
{
	adc_lh reg;
	uint16_t data;
}adc_data_union;

//USE THESE FUNCTIONS IN THE APPLICATION LAYER
void adc_on(void);
void adc_off();
unsigned short adc_sample(adc_data_union *sampled_data);
unsigned char adc_status(void);
int adc_configure(unsigned char chan_num);




#endif /*ADC_H*/