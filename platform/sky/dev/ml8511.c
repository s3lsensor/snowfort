/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/adc.h"
#include "dev/ml8511.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

#define PxDIR   P2DIR
#define PxIN    P2IN
#define PxOUT   P2OUT
#define PxSEL   P2SEL

#define EN BV(3)


static unsigned char old_pxsel, old_pxout, old_pxdir;
int ml8511_enable(){
	adc_on(); // enable adc

	// set enable pin to high
	// store old condition
	old_pxsel = PxSEL;
	old_pxout = PxOUT;
	old_pxdir = PxDIR;
	// set 
	PxSEL &= ~EN;
	PxOUT |= EN; // set GIO 2 to high to enable the pin
	PxDIR |= EN; // set GIO 2 to output mode
	
	return adc_configure(3); // set the adc pin to ADC 3
}
void ml8511_disable(){
	adc_off(); // disable adc
	// diable chip by setting GIO2 low
	PxOUT &= ~EN; // disable chipe
	// restore old mode
	PxSEL = old_pxsel;
	PxDIR = old_pxdir;
	PxOUT = old_pxout;
}

unsigned short ml8511_sample(){
	if (!adc_status()){
		ml8511_enable();
	}
	if (!adc_status()){
		printf("Cannot enable adc channel");
		return 0;
	}
	return adc_sample();
}




