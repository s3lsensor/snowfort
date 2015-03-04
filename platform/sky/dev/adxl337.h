/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang
 * Pin configuration:
 * 	U2_1	ACC
 * 	U2_9	GND
 * 	U2_3	X
 * 	U2_5	Y
 * 	U2_7	Z
 *
 */

#ifndef ADXL337_H
#define ADXL337_H

void adxl337_enable();
void adxl337_disable();

unsigned short* adxl337_sample();
unsigned short adxl337_sample_z();

#endif
