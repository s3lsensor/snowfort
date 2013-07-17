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
#define MPU_ADDRESS 0xD0


  //unsigned char rv, xx0, xx1, yy0, yy1, zz0, zz1;
  int16_t accx, accy, accz;
  int16_t gyrx, gyry, gyrz;
  //float xx, yy, zz;

  
void init_mpu6050();
void measure_mpu();

//void set_measure();
//void measure_adxl();
//void measure_itg();
//void strange_measure();
//void start();
//void write(unsigned int _c);
/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
	static struct etimer sensetimer;	
	etimer_set(&sensetimer, CLOCK_SECOND);
	uint8_t rv;
	//WDTCTL = WDTPW+WDTHOLD;
	// configuration -- bundle of single byte writes
	#ifdef debug
	printf("Configuring ADXL registers:\n");
	#endif
	init_mpu6050();
	
	// Check the configuration registers:
	
	while(1){
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&sensetimer));
		etimer_reset(&sensetimer);
		// testing i2c channel by reading whoami register, standard output should be 229
		#ifdef debug
		printf("Accessing WHOAMI register...\n");
		#endif
		rv = read_(MPU_ADDRESS, 0x75, 0);
		printf("%d \n", rv);		

		measure_mpu();

		printf("Accel value: %d\tY value: %d\tZ value: %d\n", accx, accy, accz);

		printf("Gyro value: %d\tY value: %d\tZ value: %d\n", gyrx, gyry, gyrz);


	}
 	PROCESS_END();
}

void init_mpu6050(){
	write_(MPU_ADDRESS, 0x6B, 0x01); // clear sleep bit, set clock to 0x01 (x-gyro)
	write_(MPU_ADDRESS, 0x1B, 0x00); // fs_250 for gyro
	write_(MPU_ADDRESS, 0x1C, 0x00); // fs_2 for accel
}



void set_measure(){
}

void measure_mpu(){ 

	unsigned char measurevector[14];
	read_multibyte(MPU_ADDRESS, 0x3B, 14, measurevector);

	accx = 0;
	accx |= measurevector[0];
	accx = (accx<<8) | measurevector[1];
	
	accy = 0;
	accy |= measurevector[2];
	accy = (accy<<8) | measurevector[3];
		
	accz = 0;
	accz |= measurevector[4];
	accz = (accz<<8) | measurevector[5];
	

	gyrx = 0;
	gyrx |= measurevector[8];
	gyrx = (gyrx<<8) | measurevector[9];
	
	gyry = 0;
	gyry |= measurevector[10];
	gyry = (gyry<<8) | measurevector[11];
		
	gyrz = 0;
	gyrz |= measurevector[12];
	gyrz = (gyrz<<8) | measurevector[13];
	
}


void strange_measure(){ // non_standard reading. accessing each axis separately

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
