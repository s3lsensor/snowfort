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
//#include "i2c.h"
#include <stdio.h> /* For printf() */
#include <dev/spi.h>
///////////////////////////////////////////////////////////////////////////////
#define I2C_PxDIR   P3DIR
#define I2C_PxIN    P3IN
#define I2C_PxOUT   P3OUT
#define I2C_PxSEL   P3SEL
#define SDA       1
#define SCL       3
/*
 * SDA == P3.1
 * SCL == P3.3
 */
 
 
 
#define SDA_0()   (I2C_PxDIR |=  BV(SDA))		/* SDA Output */
#define SDA_1()   (I2C_PxDIR &= ~BV(SDA))		/* SDA Input */
#define SDA_IS_1  (I2C_PxIN & BV(SDA))

#define SCL_0()   (I2C_PxDIR |=  BV(SCL))		/* SCL Output */
#define SCL_1()   (I2C_PxDIR &= ~BV(SCL))		/* SCL Input */
#define SCL_IS_1  (I2C_PxIN & BV(SCL))

/*
 * Should avoid infinite looping while waiting for SCL_IS_1. xxx/bg
 */
#define SCL_WAIT_FOR_1() do{}while (!SCL_IS_1)

#define delay_4_7us() do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); }while(0)

#define delay_4us()   do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); }while(0)

static unsigned char old_pxsel, old_pxout, old_pxdir;
///////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();
    WDTCTL = WDTPW+WDTHOLD;
  spi_busy = 1;
int i, j;
//
P3DIR = 0xFF;
 printf("process starts\n");

for(;;){
P3OUT = 0xFF;
	for(i=0; i=100; i++){
		_NOP();
		}
P3OUT = 0x00;
	for(i=0; i=100; i++){
		_NOP();
	}
}
//restore flags
printf("after infinite loop;");


  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
