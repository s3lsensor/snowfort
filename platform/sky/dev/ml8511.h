/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: analog output, need a adc converter

 * usage: Dummy file for sensor ml8511
 */
#ifndef ML8511_H
#define ML8511_H

int ml8511_enable();
void ml8511_disable();

unsigned short ml8511_sample();

#endif /*ML8511_H*/
