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
  int x, y, z;
  float xx, yy, zz;
// configuration -- bundle of single byte writes
i2c_enable();

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

		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x1D);
		i2c_write(0x40);		//0x40 - thresh-tap
		i2c_stop();
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x1E);
		i2c_write(0x00);		//0x0
		i2c_stop();

		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x1F);
		i2c_write(0x00);		//0x0
		i2c_stop();

		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x20);
		i2c_write(0x00);		//0x0 - three offsets
		i2c_stop();

		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x21);
		i2c_write(0x7f);		//0x7f duration
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x22);
		i2c_write(0x30);		//0x30 - latency
		i2c_stop();
		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x23);
		i2c_write(0x7f);		//0x7f - window
		i2c_stop();

		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x24);
		i2c_write(0x02);		//0x2 thresh_act
		i2c_stop();
		
				
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x25);
		i2c_write(0x01);		//0x1 thresh_inact
		i2c_stop();
				
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x26);
		i2c_write(0xff);		//0xff time_inact
		i2c_stop();
				
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x27);
		i2c_write(0xff);		//0xff act_inact_ctl
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x28);
		i2c_write(0x05);		//0x05 thresh_ff
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x29);
		i2c_write(0x14);		//0x14 time_ff
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x2A);
		i2c_write(0x07);		//0x7 tap_axes
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x2B);
		i2c_write(0x00);		//0x0 act_tap_st
		i2c_stop();
		
				
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x2C);
		i2c_write(0x0a);		//0x0a bw_rate
		i2c_stop();		
// */
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x2D);
		i2c_write(0x08);		//0x08 power_ctl
		i2c_stop();		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x2E);
		i2c_write(0x00);		//0x0 int_enable
		i2c_stop();

// */

i2c_disable();
// Major loop for reading
do{
		i2c_enable();
		
/*		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x1D);
		i2c_write(0x40);//0x40 - thresh-tap
		i2c_write(0x00);//0x0
		i2c_write(0x00);//0x0
		i2c_write(0x00);//0x0 - three offsets
		i2c_write(0x7f);//0x7f duration
		i2c_write(0x30);//0x30 - latency
		i2c_write(0x7f);//0x7f - window
		i2c_write(0x02);//0x2 thresh_act
		i2c_write(0x01);//0x1 thresh_inact
		i2c_write(0xff);//0xff time_inact
		i2c_write(0xff);//0xff act_inact_ctl
		i2c_write(0x05);//0x05 thresh_ff
		i2c_write(0x14);//0x14 time_ff
		i2c_write(0x07);//0x7 tap_axes
		i2c_write(0x00);//0x0 act_tap_st
		i2c_write(0x0a);//0x0a bw_rate
		i2c_write(0x08);//0x08 power_ctl
		i2c_write(0x00);//0x0 int_enable
		i2c_stop();
// */		
		
		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x00);
		i2c_stop();
		i2c_start();
		i2c_write(0xA7);
		rv = i2c_read(0);
		i2c_stop();


		i2c_start();
		i2c_write(0xA6);
		i2c_write(0x32);
		i2c_stop();
		i2c_start();
		i2c_write(0xA7);
		x0=i2c_read(1);
		x1=i2c_read(1);
		y0=i2c_read(1);
		y1=i2c_read(1);
		z0=i2c_read(1);
		z1=i2c_read(0);
		i2c_stop();
		
		i2c_disable();
		
		x = 0;
		x |= x1;
		x = (x<<8) | x0;
		xx=x*0.0078;
		y = 0;
		y |= y1;
		y = (y<<8) | y0;
		yy=y*0.0078;
		z = 0;
		z |= z1;
		z = (z<<8) | z0;
		zz=z*0.0078;
		
		printf("SELF: %d\nX: %d %d \nY: %d %d \nZ: %d %d \n", rv, x0, x1, y0, y1, z0, z1);
		printf("X value: %d\tY value: %d\tZ value: %d\n", x, y, y);

		for(rv =1 ; rv<=25; rv++){
			_NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP(); _NOP();
		}

}while(1);
  PROCESS_END();
}

