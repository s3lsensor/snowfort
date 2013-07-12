/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * $Id: hello-world.c,v 1.1 2006/10/02 21:46:46 adamdunkels Exp $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look
 * \author
 *         Adam Dunkels <adam@sics.se>
 */

#include "contiki.h"
#include "i2c.h"
#include "spi.h"
#include <stdio.h> /* For printf() */
#include "sys/etimer.h"
#define debug
#define DUR (2500)
#define ADXL_ADDRESS 0xA6

  unsigned char rv, xx0, xx1, yy0, yy1, zz0, zz1;
  int x, y, z;
  float xx, yy, zz;
  
void init();
void set_measure();
void measure();
void strange_measure();
void start();
void write(unsigned int _c);
//void stop();
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
	static struct etimer sensetimer;	
	etimer_set(&sensetimer, CLOCK_SECOND);
	//WDTCTL = WDTPW+WDTHOLD;
	//int i;
	//unsigned int ind;
	// configuration -- bundle of single byte writes
	#ifdef debug
	printf("Configuring ADXL registers:\n");
	#endif
	init();
	// Check the configuration registers:
	/*for(ind=0x1D; ind<=0x2E; ind++){
		printf("TERRI %d is %d\n", ind, read_(ADXL_ADDRESS, ind, 0));
	}*/
//	const int numregisters = 16;
	unsigned char configvector[29];
	unsigned char startregister = 0x1D;
	read_multibyte(ADXL_ADDRESS, startregister, 29, configvector);
	int ii;
	for(ii = 0; ii<29; ii++){
		printf("%d register: %d \n", ii+startregister, configvector[ii]);
	}
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sensetimer));
		etimer_reset(&sensetimer);
		// testing i2c channel by reading whoami register, standard output should be 229
		#ifdef debug
		printf("Accessing WHOAMI register...\n");
		#endif
		rv = read_(ADXL_ADDRESS, 0x00, 0);
		
		if(rv == 229)
			printf("i2c working correctly, this sensor is 229\n");
		else{
			printf("ERROR!ERROR!ERROR!ERROR!ERROR!ERROR!ERROR!\n");
		}

		//#ifdef debug
		//printf("Setting measurement mode before each measurement\n");
		//#endif		
		//enable measurement before each reading;
		//printf("current mode %d\n", read_(ADXL_ADDRESS, 0x2D, 0));
		// STRANGE: set_meaaure() and single write_can never work, init() works okay
		//set_measure();
		//init();
		// */
			
		//for(ind=0x1D; ind<=0x31; ind++){
		//	printf("%d is %d\n", ind, read_(ADXL_ADDRESS, ind, 0));
		//}

		//	unsigned char comparevector[5];
		//	read_multibyte(ADXL_ADDRESS, 0x1D, 5, comparevector);
		//	printf("COMPARE:\n %d \n %d \n %d \n %d \n %d\n", comparevector[0],comparevector[1],comparevector[2],comparevector[3],comparevector[4]);



		// get acceleration on x, y, z axis
		measure();
		//printf("REGULAR\n");

//		strange_measure();
//		printf("STRANGE\n");


		//append the 2 bytes		
		x = 0;
		x |= xx1;
		#ifdef debug
		//printf("debugging...\nx0: %d\tx1: %d\n", x, xx1);
		#endif
		x = (x<<8) | xx0;
		//xx=x*0.0078;
		
		y = 0;
		y |= yy1;
		#ifdef debug
		//printf("y0: %d\ty1: %d\n", y, yy1);
		#endif
		y = (y<<8) | yy0;
		//yy=y*0.0078;
		
		z = 0;
		z |= zz1;
		#ifdef debug
		//printf("z0: %d\tz1: %d\n", z, zz1);
		#endif
		z = (z<<8) | zz0;
		//zz=z*0.0078;

		printf("X: %d %d \nY: %d %d \nZ: %d %d \n",  xx0, xx1, yy0, yy1, zz0, zz1);
		printf("X value: %d\tY value: %d\tZ value: %d\n", x, y, z);

		/*for(i =1 ; i<=25000; i++){
			_NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		}*/

		//printf("%d\n\n\n", i++);
	}
 	PROCESS_END();
}


void init(){

	//0x40 - thresh-tap
	//0x0
	//0x0
	//0x0 - three offsets
	//0x7f duration
	//0x30 - latency
	//0x7f - window
	//0x2 thresh_act
	//0x1 thresh_inact
	//0xff time_inact
	//0xff act_inact_ctl
	//0x05 thresh_ff
	//0x14 time_ff
	//0x7 tap_axes
	//0x0 act_tap_st
	//0x0a bw_rate
	//0x08 power_ctl
	//0x0 int_enable
	//0x01 data format

	write_(ADXL_ADDRESS, 0x1D, 0x40);			
	write_(ADXL_ADDRESS, 0x1E, 0x00);
	write_(ADXL_ADDRESS, 0x1F, 0x00);
	write_(ADXL_ADDRESS, 0x20, 0x00);		
	write_(ADXL_ADDRESS, 0x21, 0x7f);
	write_(ADXL_ADDRESS, 0x22, 0x30);		
	write_(ADXL_ADDRESS, 0x23, 0x7f);
	write_(ADXL_ADDRESS, 0x24, 0x02);		
	write_(ADXL_ADDRESS, 0x25, 0x01);
	write_(ADXL_ADDRESS, 0x26, 0xff);
	write_(ADXL_ADDRESS, 0x27, 0xff);
	write_(ADXL_ADDRESS, 0x28, 0x05);
	write_(ADXL_ADDRESS, 0x29, 0x14);
	write_(ADXL_ADDRESS, 0x2A, 0x07);
	write_(ADXL_ADDRESS, 0x2B, 0x00);
	//write_(ADXL_ADDRESS, 0x2C, 0x0A);//100 Hz sampling
	write_(ADXL_ADDRESS, 0x2C, 0x07);//12.5 Hz sampling
	write_(ADXL_ADDRESS, 0x2D, 0x08);		
	write_(ADXL_ADDRESS, 0x2E, 0x00);		
//	write_(ADXL_ADDRESS, 0x31, 0x00);//not full_res, +- 2g
	write_(ADXL_ADDRESS, 0x31, 0x04);//full_res, +- 2g

}

void set_measure(){
	//int i;
/*
		start();
		write(ADXL_ADDRESS);
		write(0x2D);
		write(0x08);		//0x08 power_ctl
		stop();	
// */	
	write_(ADXL_ADDRESS, 0x2D, 0x08);
/*	
#ifdef debug
printf("wait for measure.\n");
#endif	
		for(i =1 ; i<=DUR; i++){
			_NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		}
		printf("%d", i);
		*/
}

void measure(){ // standard data accessing according to the datasheet

	unsigned char measurevector[6];
	read_multibyte(ADXL_ADDRESS, 0x32, 6, measurevector);
	xx0 = measurevector[0];
	xx1 = measurevector[1];
	yy0 = measurevector[2];
	yy1 = measurevector[3];
	zz0 = measurevector[4];
	zz1 = measurevector[5];


/*		start();
		write(ADXL_ADDRESS);
		write(0x32);
		i2c_stop();
		i2c_start();
		write(0xA7);
		xx0=i2c_read(1);
		xx1=i2c_read(1);
		yy0=i2c_read(1);
		yy1=i2c_read(1);
		zz0=i2c_read(1);
		zz1=i2c_read(0);
		stop();*/
		

}

void strange_measure(){ // non_standard reading. accessing each axis separately

/*
		start();
		write(ADXL_ADDRESS);
		write(0x32);
		i2c_stop();
		i2c_start();
		write(0xA7);
		xx0=i2c_read(0);
		stop();

		start();
		write(ADXL_ADDRESS);
		write(0x33);
		i2c_stop();
		i2c_start();
		write(0xA7);
		xx1=i2c_read(0);
		stop();
			
		start();
		write(ADXL_ADDRESS);
		write(0x34);
		i2c_stop();
		i2c_start();
		write(0xA7);
		yy0=i2c_read(0);
		stop();

		start();
		write(ADXL_ADDRESS);
		write(0x35);
		i2c_stop();
		i2c_start();
		write(0xA7);
		yy1=i2c_read(0);
		stop();
		
		start();
		write(ADXL_ADDRESS);
		write(0x36);
		i2c_stop();
		i2c_start();
		write(0xA7);
		zz0=i2c_read(0);
		stop();
		
		start();
		write(ADXL_ADDRESS);
		write(0x37);
		i2c_stop();
		i2c_start();
		write(0xA7);
		zz1=i2c_read(0);
		stop();		
// */
	xx0 = read_(ADXL_ADDRESS, 0x32, 0);
	xx1 = read_(ADXL_ADDRESS, 0x33, 0);
	yy0 = read_(ADXL_ADDRESS, 0x34, 0);
	yy1 = read_(ADXL_ADDRESS, 0x35, 0);
	zz0 = read_(ADXL_ADDRESS, 0x36, 0);		
	zz1 = read_(ADXL_ADDRESS, 0x36, 0);
}


void start(){
	i2c_enable();
	int suc;
	suc = i2c_start();
	while(suc == -1){
		printf("start failed!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		suc=i2c_start();
	}
}

void write(unsigned _c){
	if (i2c_write(_c)==0)
		printf("write unsuccessful!!!!!!!!!!!!!!!!!!!!!!!!\n");
}

void stop(){
	i2c_stop();
	i2c_disable();
}
