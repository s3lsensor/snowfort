/*
 * This file defines the adc functions which are architecture
 * specific and implemented in /cpu/msp430/f1xxx
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */


//USE THESE FUNCTIONS IN THE APPLICATION LAYER
void adc_on(void);
void adc_off();
unsigned short adc_sample(void);
unsigned char adc_status(void);
int adc_configure(unsigned char chan_num);


//DO NOT USE THESE FUNCTIONS IN APPLICATION LAYER
void adc_arch_on(void);
void adc_arch_off(void);
unsigned short adc_arch_sample(void);
int adc_arch_configure(unsigned char chan_num);
