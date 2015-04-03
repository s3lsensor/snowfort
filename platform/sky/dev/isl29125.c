/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/i2c.h"
#include "dev/isl29125.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

void isl29125_enable(void){
	i2c_enable();
	isl29125_config(CFG1_MODE_RGB | CFG1_10KLUX, CFG2_IR_ADJUST_HIGH, CFG_DEFAULT);
}

void isl29125_config(int8_t config1, int8_t config2, int8_t config3){
	write_(ISL_I2C_ADDR, CONFIG_1, config1);
	write_(ISL_I2C_ADDR, CONFIG_2, config2);
	write_(ISL_I2C_ADDR, CONFIG_3, config3);
}

void isl29125_setUpperThreshold(int16_t data){
	i2c_start();

	i2c_write(ISL_I2C_ADDR);
	i2c_write(THRESHOLD_HL);

	i2c_write((unsigned) 0xFF & data);
	i2c_write((unsigned) (data)>> 8);

	i2c_stop();
}

void isl29125_setLowerThreshold(uint16_t data){
	i2c_start();

	i2c_write(ISL_I2C_ADDR);
	i2c_write(THRESHOLD_LL);

	i2c_write((unsigned) 0xFF & data);
	i2c_write((unsigned) (data)>> 8);

	i2c_stop();
}

int16_t isl29125_readUpperThreshold(void){
	unsigned rv[2];
	read_multibyte(ISL_I2C_ADDR, THRESHOLD_HL, 2, rv);

	int16_t rlt;
	rlt = (rv[1] << 8) | rv[0];

	return rlt;
}

int16_t isl29125_readLowerThreshold(void){
	unsigned rv[2];
	read_multibyte(ISL_I2C_ADDR, THRESHOLD_LL, 2, rv);

	int16_t rlt;
	rlt = (rv[1] << 8) | rv[0];

	return rlt;
}

int16_t isl29125_sample_red(void){
	int16_t rlt;
	rlt = (int16_t) read_(ISL_I2C_ADDR, RED_L, 1);
	rlt = ((int16_t) read_(ISL_I2C_ADDR, RED_H, 1)) << 8 | rlt;

	return rlt;
}


int16_t isl29125_sample_blue(void){
	int16_t rlt;
	rlt = (int16_t) read_(ISL_I2C_ADDR, BLUE_L, 1);
	rlt = ((int16_t) read_(ISL_I2C_ADDR, BLUE_H, 1)) << 8 | rlt;

	return rlt;
}


int16_t isl29125_sample_green(void){
	int16_t rlt;
	rlt = (int16_t) read_(ISL_I2C_ADDR, GREEN_L, 1);
	rlt = ((int16_t) read_(ISL_I2C_ADDR, GREEN_H, 1)) << 8 | rlt;

	return rlt;
}

int8_t isl29125_readStatus(void){
	return read_(ISL_I2C_ADDR, STATUS, 1);
}


isl29125_union isl29125_sample(void){
	isl29125_union sample;
	int16_t tmp;
	tmp = isl29125_sample_green();
	sample.green.h = (int8_t) tmp >> 8;
	sample.green.l = (int8_t) tmp & 0xFF;
	tmp = isl29125_sample_blue();
	sample.blue.h = (int8_t) tmp >> 8;
	sample.blue.l = (int8_t) tmp & 0xFF;
	tmp = isl29125_sample_red();
	sample.red.h = (int8_t) tmp >> 8;
	sample.red.l = (int8_t) tmp & 0xFF;

	return sample;
}
void print_isl29125_sample(isl29125_union sample){
	uart1_writeb(sample.red.h);
	uart1_writeb(sample.red.l);
	uart1_writeb(sample.blue.h);
	uart1_writeb(sample.blue.l);
	uart1_writeb(sample.green.h);
	uart1_writeb(sample.green.l);
}

void print_isl29125_data(isl29125_data data)
{
	uart1_writeb(data.h);
	uart1_writeb(data.l);
}

//void print_isl29125_sample(int16_t data){
//	uart1_writeb((unsigned char) data >> 8);
//	uart1_writeb((unsigned char) data & 0xFF);
//}
