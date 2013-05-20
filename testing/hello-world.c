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
#include <stdio.h> /* For printf() */

///////////////////////////////////////////////////////////////////////////////
#define SDA_0()   (I2C_PxDIR |=  BV(SDA))		/* SDA Output */
#define SDA_1()   (I2C_PxDIR &= ~BV(SDA))		/* SDA Input */
#define SDA_IS_1  (I2C_PxIN & BV(SDA))

#define SCL_0()   (I2C_PxDIR |=  BV(SCL))		/* SCL Output */
#define SCL_1()   (I2C_PxDIR &= ~BV(SCL))		/* SCL Input */
#define SCL_IS_1  (I2C_PxIN & BV(SCL))
///////////////////////////////////////////////////////////////////////////////

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

 unsigned rv;
 printf("process starts\n");

//check and receive
i2c_enable();
	printf("i2c enabled\n");
/*-------I2C:	read data for X1----------------------------------------------*/
//start
i2c_start();
//slave address-write
if(!i2c_write(0x0A))
	printf("write slave address first byte transmit fail!\n");
if(!i2c_write(0x03))
	printf("write slave address second byte transmit fail!\n");
//register address
if(i2c_write(0x03))
	printf("register first byte transmit fail\n");
if(!i2c_write(0x03))
	printf("register second byte transmit fail\n");
//stop-start
i2c_stop();
i2c_start();
//slave address-read
if(!i2c_write(0x0B))
	printf("read slave address second byte transmit fail!\n");
if(!i2c_write(0x03))
	printf("read slave address second byte transmit fail!\n");
//read data
rv = i2c_read(0);
//NACK

//stop
i2c_stop();

//disable
i2c_disable();

  printf("%d", rv);
  
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
