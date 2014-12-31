/*
 * This file defines the driver for ms5803
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */

/*
 * Pin connection: VCC -> U2-1
		   GND -> U2-9
		   SCL -> U28-3
		   SDA -> U28-4
		   PS -> VCC
		   CS -> GND  : I2C address will be 0x77 

 * usage: init before use to get the coefficients. reset everytime the sensor restarts. 
 */
#ifndef ISL29125_H
#define ISL29125_H

#define delay_1ms()   do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); }while(250)

#define ISL_I2C_ADDR 0x44

#define DEVICE_ID 0x00
#define CONFIG_1 0x01
#define CONFIG_2 0x02
#define CONFIG_3 0x03
#define THRESHOLD_LL 0x04
#define THRESHOLD_LH 0x05
#define THRESHOLD_HL 0x06
#define THRESHOLD_HH 0x07
#define STATUS 0x08 
#define GREEN_L 0x09 
#define GREEN_H 0x0A
#define RED_L 0x0B
#define RED_H 0x0C
#define BLUE_L 0x0D
#define BLUE_H 0x0E


#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1


typedef struct
{
    int8_t hByte;
    int8_t mByte;
    int8_t lByte;
}ms5803_data;

typedef struct
{
	int32_t pressure;
	int32_t temperature;
}ms5803_union

// #define MS5803_DATA_SIZE (sizeof(ms5803_data)/sizeof(uint8_t))

int16_t* isl29125_init();
int isl29125_reset();
int isl29125_config(int8_t cmd);
int isl29125_sample_red(int8_t precision, int16_t *coeff);
int isl29125_sample_blue(int8_t precision, int16_t *coeff);
int isl29125_sample_green(int8_t precision, int16_t *coeff);


void print_isl29125_sample(ms5803_union samples);

#endif
