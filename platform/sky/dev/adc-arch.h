/*
 * Author: Yizheng Liao
 */

#ifndef ADC_ARCH_H
#define ADC_ARCH_H


//DO NOT USE THESE FUNCTIONS IN APPLICATION LAYER
void adc_arch_on(void);
void adc_arch_off(void);
unsigned int adc_arch_sample(void);
int adc_arch_configure(unsigned char chan_num);

#endif /* ADC_ARCH_H */