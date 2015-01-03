/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: analog output, need a adc converter
 * x -- ADC_x
 * y -- ADC_y
 * z -- ADC_z
 * usage: dummy files for sensor adxl337
 */

#include "contiki.h"

#include <stdio.h>
#include "dev/adxl337.h"
#include "dev/adc.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


#define ADC_x 1
#define ADC_y 2
#define ADC_z 3

void adxl337_enable(){
	adc_on();
}
void adxl337_disable(){
	adc_off();
}

unsigned short* adxl337_sample(){
	unsigned short rlt[3] = {0};
	if(adc_configure(ADC_x)){
		rlt[0] = adc_sample();
	}else{
		printf("Cannot sample x!!");
	}


	if(adc_configure(ADC_y)){
		rlt[1] = adc_sample();
	}else{
		printf("Cannot sample x!!");
	}

	if(adc_configure(ADC_z)){
		rlt[2] = adc_sample();
	}else{
		printf("Cannot sample x!!");
	}
}

unsigned short adxl337_sample_z(){
	if(adc_configure(ADC_z)){
		return adc_sample();
	}
}

