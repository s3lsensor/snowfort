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
#include "dev/ms5803.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// Initialize the sensor and return coefficients
int16_t* ms5803_init(){
	int16_t coefficients[8];
	for (int i=0; i<8;i++){
		ms5803_send(CMD_PROM+i*2);
		start();
		if !write(MS5803_ADDR){
			printf("I2C write failure...");
			i2c_stop();
			return NULL;
		}
		int8_t highByte = i2c_read(1);
		int8_t lowByte = i2c_read(1);
		coefficients[i] = (highByte<<8)|lowByte;
	}
	return coefficients;
}

// reset the sensor
int ms5803_reset(){
	ms5803_send(CMD_RESET);
	delay_1ms();delay_1ms();delay_1ms();
	return 0;
}

// send byte to sensor
int ms5803_send(int8_t cmd){
	int ret = 0;
	i2c_start();
	if (i2c_write(MS5803_ADDR)}{
	}else{
		printf("I2C write failure...");
		ret = 1;
	}
	if (i2c_write(cmd)){
	}else{
		printf("I2C write failure...");
		ret = 1;
	}
	i2c_stop();
	return ret;
}

// sample temperature and pressure, temperature in Celsius is (returned value)/10
ms5803_union ms5803_sample(int8_t precision, int16_t *coeff){
	int32_t temp_raw = ms5803_ADCConv(TEMPERATURE, precision, coeff);
	int32_t pree_raw = ms5803_ADCConv(PRESSURE, precision, coeff);

	int32_t temp_calc, press_calc;
	int32_t dT = temp_raw - ((int32_t)coeff[5] << 8);

	temp_calc = (((int64_t)dT * coefficient[6]) >> 23) + 2000;

	int64_t T2, OFF2, SENS2, OFF, SENS;
	if (temp_calc < 2000){
		T2 = 3 * (((int64_t)dT * dT) >> 33);
		OFF2 = 3 * ((temp_calc - 2000) * (temp_calc - 2000)) / 2;
		SENS2 = 5 * ((temp_calc - 2000) * (temp_calc - 2000)) / 8;
		if(temp_calc < -1500)
		// If temp_calc is below -15.0C
		{
		OFF2 = OFF2 + 7 * ((temp_calc + 1500) * (temp_calc + 1500));
		SENS2 = SENS2 + 4 * ((temp_calc + 1500) * (temp_calc + 1500));
		}
	}else{
		T2 = 7 * ((uint64_t)dT * dT)/pow(2,37);
		OFF2 = ((temp_calc - 2000) * (temp_calc - 2000)) / 16;
		SENS2 = 0;
	}

	OFF = ((int64_t)coeff[2] << 16) + (((coeff[4] * (int64_t)dT)) >> 7);
	SENS = ((int64_t)coeff[1] << 15) + (((coeff[3] * (int64_t)dT)) >> 8);
	temp_calc = temp_calc - T2;
	OFF = OFF - OFF2;
	SENS = SENS - SENS2;
	// Now lets calculate the pressure
	press_calc = (((SENS * press_raw) / 2097152 ) - OFF) / 32768;

	ms5803_union rlt;
	rlt.pressure = press_calc;
	rlt.temperature = temp_calc;

	return rlt;
}


// get adc conversion
int32_t ms5803_ADCConv(int8_t type, int8_t precision, int16_t *coeff){
	int32_t rlt;
	ms5803_data tmpData;
	ms5803_send(CMD_ADC_CONV + type + precision);
	
	delay_1ms();
	switch(precision){
		case ADC_256 : delay_1ms(); break;
		case ADC_512 : delay_1ms();delay_1ms();delay_1ms(); break;
		case ADC_1024: delay_1ms();delay_1ms();delay_1ms();delay_1ms(); break;
		case ADC_2048: delay_1ms();delay_1ms();delay_1ms();delay_1ms();delay_1ms();delay_1ms(); break;
		case ADC_4096: delay_1ms();delay_1ms();delay_1ms();delay_1ms();delay_1ms();
			       delay_1ms();delay_1ms();delay_1ms();delay_1ms();delay_1ms(); break; 
	}

	ms5803_send(CMD_ADC_READ);
	i2c_start();
	i2c_write(MS5803_ADDR);
	tmpData.hByte = i2c_read(1);
	tmpData.mByte = i2c_read(1);
	tmpData.lByte = i2c_read(1);
	i2c_stop();

	rlt = ((int32_t) tmpData.highByte << 16) + ((int32_t) tmpData.mByte << 8) + tmpData.lowByte
	return rlt;
}

// print all sample results
void print_ms5803_sample(ms5803_union sample){
	int32_t tmp = sample.pressure;
	int8_t lowByte = (tmp & 0x0000FF)
	int8_t midByte = ((tmp << 8) & 0x0000FF)
	int8_t higByte = ((tmp << 16) & 0x0000ff)
	uart1_writeb((unsigned char) higByte);
	uart1_writeb((unsigned char) midByte);
	uart1_writeb((unsigned char) lowByte);


	int32_t tmp = sample.temperature;
	int8_t lowByte = (tmp & 0x0000FF)
	int8_t midByte = ((tmp << 8) & 0x0000FF)
	int8_t higByte = ((tmp << 16) & 0x0000ff)
	uart1_writeb((unsigned char) higByte);
	uart1_writeb((unsigned char) midByte);
	uart1_writeb((unsigned char) lowByte);


}



