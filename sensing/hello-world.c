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



/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
  unsigned rv, x0, x1, y0, y1, z0, z1;
// configuration -- bundle of single byte writes
i2c_enable();


/*
	//disable act/inact mode: 0x27 -- 0x00
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x27);
	i2c_write(0x00);
	i2c_stop();
	//disable tap mode: 0x2A -- 0x00
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2A);
	i2c_write(0x00);
	i2c_stop();
	//POWER_CTL: 0x2D -- link/0; auto_sleep/0; measure/1; sleep_bit/0; wakeup_bits/00; 00001000/0x08
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2D);
	i2c_write(0x08);
	i2c_stop();
	//INT_ENABLE: 0x2E -- 0x00
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2E);
	i2c_write(0x00);
	i2c_stop();
	//data format: 0x31 -- self_test/0; spi/0; int_invert/X; full_res/0; range bits/01 (4g)	00000001/0x01
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x31);
	i2c_write(0x01);
	i2c_stop();
	//FIFO_CTL: 0x38 -- 00/bypass; trigger/X; 0x00
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x38);
	i2c_write(0x00);
	i2c_stop();
// */


	//POWER_CTL: 0x2D -- link/0; auto_sleep/0; measure/1; sleep_bit/0; wakeup_bits/00; 00001000/0x08
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2D);
	i2c_write(0x08);
	i2c_stop();
	
	//data format: 0x31 -- self_test/0; spi/0; int_invert/X; full_res/0; range bits/01 (4g)	00000001/0x01
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x31);
	i2c_write(0x01);
	i2c_stop();
	
i2c_disable();
// Major loop for reading
do{
		i2c_enable();
		
		
		// WHOAMI reference
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x00);
		i2c_stop();
		i2c_start();
		i2c_write(0xA7);
		rv = i2c_read(0);
		i2c_stop();
		printf("Self: %d\n", rv);
		
// reference working okay

	//// configure before every reading
	//POWER_CTL: 0x2D -- link/0; auto_sleep/0; measure/1; sleep_bit/0; wakeup_bits/00; 00001000/0x08
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2D);
	i2c_write(0x08);
	i2c_stop();
	
	//data format: 0x31 -- self_test/0; spi/0; int_invert/X; full_res/0; range bits/01 (4g)	00000001/0x01
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x31);
	i2c_write(0x01);
	i2c_stop();


	// read each register with different single byte reading.
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for X0;
		i2c_write(0x32);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		x0 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for X1;
		i2c_write(0x33);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		x1 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for Y0;
		i2c_write(0x34);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		y0 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for Y1;
		i2c_write(0x35);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		y1 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();		
		
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for Z0;
		i2c_write(0x36);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		z0 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		//start
		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for Z1;
		i2c_write(0x37);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		z1 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		printf("X: %d %d\nY: %d %d\nZ: %d %d\n", x0, x1, y0, y1, z0, z1);
// */		
		

		
// suppose to be, but not working	
/*

	//POWER_CTL: 0x2D -- link/0; auto_sleep/0; measure/1; sleep_bit/0; wakeup_bits/00; 00001000/0x08
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x2D);
	i2c_write(0x08);
	i2c_stop();
	
	//data format: 0x31 -- self_test/0; spi/0; int_invert/X; full_res/0; range bits/01 (4g)	00000001/0x01
	i2c_start();
	i2c_write(0xA6);
	i2c_write(0x31);
	i2c_write(0x01);
	i2c_stop();

		i2c_start();
		//slave address-write
		i2c_write(0xA6);
		//register address for Z1;
		i2c_write(0x37);
		//stop-start
		i2c_stop();
		i2c_start();
		//slave address-read
		i2c_write(0xA7);
		//read data
		x0 = i2c_read(1);
		x1 = i2c_read(1);
		y0 = i2c_read(1);
		y1 = i2c_read(1);
		z0 = i2c_read(1);
		z1 = i2c_read(0);
		//NACK
		//stop
		i2c_stop();
		
		printf("X: %d %d\nY: %d %d\nZ: %d %d\n", x0, x1, y0, y1, z0, z1);
// */		
		
		//disable
		i2c_disable();
		
		for(rv =1 ; rv<=25; rv++){
			_NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		}


}while(1);
  PROCESS_END();
}

