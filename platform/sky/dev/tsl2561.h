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
#ifndef TSL2561_H
#define TSL2561_H

#define delay_1ms()   do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); }while(250)

#define TSL2561_ADDR   0x39

#define TSL2561_CMD           0x80
#define TSL2561_CMD_CLEAR     0xC0
#define	TSL2561_REG_CONTROL   0x00
#define	TSL2561_REG_TIMING    0x01
#define	TSL2561_REG_THRESH_L  0x02
#define	TSL2561_REG_THRESH_H  0x04
#define	TSL2561_REG_INTCTL    0x06
#define	TSL2561_REG_ID        0x0A
#define	TSL2561_REG_DATA_0    0x0C
#define	TSL2561_REG_DATA_1    0x0E

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1




#define MS5803_ADDR 0x77

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

int16_t* tsl2561_init();
int tsl2561_poweron();
int tsl2561_powerdonw(int8_t cmd);
int tsl2561_settiming();
int tsl2561_sample(int8_t precision, int16_t *coeff);


void print_tsl2561_sample(ms5803_union samples);

#endif
