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
#include <stdio.h> /* For printf() */



/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
  PROCESS_BEGIN();

 unsigned rv;

 unsigned rd = 1; 
 
 printf("process starts\n");
// read X1 data on ADXL
  P3SEL |= 0x0a;                            // Assign I2C pins to module
  U0CTL |= I2C + SYNC;                      // Switch USART0 to I2C mode
  U0CTL &= ~I2CEN;                          // Recommended I2C init procedure
  //I2CTCTL = I2CSSEL_2;                      // SMCLK
  I2CSCLH = 0x02;                           // High period of SCL
  I2CSCLL = 0x02;                           // Low period of SCL
  I2CNDAT = 0x01;                           // Transmit one byte
  U0CTL |= MST;									  // Master Mode
  I2CIE = ARDYIFG;								  // data register ready interrupt enable
  // Transmit register address
  I2CSA = 0xA6;                             // Slave write address
  I2CDRB = 0x33;									  // reading register address: X1 data
  I2CTCTL = I2CSSEL_2 + I2CTRX + I2CSTT + I2CSTP;      // Transmit, ST, SP (clears MST)
  printf("waiting for register address transmission. I2CIV = %d\n", I2CIV);
  while(I2CIV!=0x08);
  rd = 1;							  // wait for transmit ends
  printf("register address transmitted\n");
  //receive register data
  I2CSA = 0xA7; 									  // Slave read address
  I2CTCTL = I2CSSEL_2 + I2CSTT + I2CSTP;	  // receive X1 data
  printf("waiting for data from sensor. I2CIV = %d\n", I2CIV);
  while(I2CIV!=0x08);							  // wait for transmit ends
  rd = 1; 
  rv = I2CDRB;
 
 
  printf("X1 data: %d\n", rv);
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/*
#pragma vector=USART0TX_VECTOR
__interrupt void USART0 (void)
{

  switch( I2CIV )
  {
   case 0x08:
	 rd = 0;
    break;

  }
}
*/
