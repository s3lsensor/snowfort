/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */

#ifndef ADXL337_H
#define ADXL337_H

void adxl337_enable();
void adxl337_disable();

unsigned short* adxl337_sample();
unsigned short adxl337_sample_z();

#endif
