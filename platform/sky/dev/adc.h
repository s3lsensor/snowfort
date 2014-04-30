/*
 * This file defines the adc functions which are architecture
 * specific and implemented in /cpu/msp430/f1xxx
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */

#ifndef ADC_H
#define ADC_H

//USE THESE FUNCTIONS IN THE APPLICATION LAYER
void adc_on(void);
void adc_off();
unsigned short adc_sample(void);
unsigned char adc_status(void);
int adc_configure(unsigned char chan_num);




#endif /*ADC_H*/