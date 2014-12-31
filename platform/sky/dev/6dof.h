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
#ifndef 6DOF_H
#define 6DOF_H

#define delay_1ms()   do{ _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); _NOP(); _NOP(); \
                          _NOP(); _NOP(); }while(250)

//ADXL337 Register Map
#define ADXL_ADDR	0xA6
#define	DEVID			0x00	//Device ID Register
#define THRESH_TAP		0x1D	//Tap Threshold
#define	OFSX			0x1E	//X-axis offset
#define	OFSY			0x1F	//Y-axis offset
#define	OFSZ			0x20	//Z-axis offset
#define	DUR				0x21	//Tap Duration
#define	Latent			0x22	//Tap latency
#define	Window			0x23	//Tap window
#define	THRESH_ACT		0x24	//Activity Threshold
#define	THRESH_INACT	0x25	//Inactivity Threshold
#define	TIME_INACT		0x26	//Inactivity Time
#define	ACT_INACT_CTL	0x27	//Axis enable control for activity and inactivity detection
#define	THRESH_FF		0x28	//free-fall threshold
#define	TIME_FF			0x29	//Free-Fall Time
#define	TAP_AXES		0x2A	//Axis control for tap/double tap
#define ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define	BW_RATE			0x2C	//Data rate and power mode control
#define POWER_CTL		0x2D	//Power Control Register
#define	INT_ENABLE		0x2E	//Interrupt Enable Control
#define	INT_MAP			0x2F	//Interrupt Mapping Control
#define	INT_SOURCE		0x30	//Source of interrupts
#define	DATA_FORMAT		0x31	//Data format control
#define DATAX0			0x32	//X-Axis Data 0
#define DATAX1			0x33	//X-Axis Data 1
#define DATAY0			0x34	//Y-Axis Data 0
#define DATAY1			0x35	//Y-Axis Data 1
#define DATAZ0			0x36	//Z-Axis Data 0
#define DATAZ1			0x37	//Z-Axis Data 1
#define	FIFO_CTL		0x38	//FIFO control
#define	FIFO_STATUS		0x39	//FIFO status

//Power Control Register Bits
#define WU_0		(1<<0)	//Wake Up Mode - Bit 0
#define	WU_1		(1<<1)	//Wake Up mode - Bit 1
#define SLEEP		(1<<2)	//Sleep Mode
#define	MEASURE		(1<<3)	//Measurement Mode
#define AUTO_SLP	(1<<4)	//Auto Sleep Mode bit
#define LINK		(1<<5)	//Link bit





//ITG3200 Register Map
#define ITG_ADDR	0xD0 //AD0 is tied to gnd
#define WHO_AM_I	0x00
#define SMPLRT_DIV	0x15
#define	DLPF_FS		0x16
#define INT_CFG		0x17
#define INT_STATUS	0x1A
#define	TEMP_OUT_H	0x1B
#define	TEMP_OUT_L	0x1C
#define GYRO_XOUT_H	0x1D
#define	GYRO_XOUT_L	0x1E
#define GYRO_YOUT_H	0x1F
#define GYRO_YOUT_L	0x20
#define GYRO_ZOUT_H	0x21
#define GYRO_ZOUT_L	0x22
#define	PWR_MGM		0x3E


#define DLPF_CFG_0	(1<<0)
#define DLPF_CFG_1	(1<<1)
#define DLPF_CFG_2	(1<<2)
#define DLPF_FS_SEL_0	(1<<3)
#define DLPF_FS_SEL_1	(1<<4)


#define PWR_MGM_CLK_SEL_0	(1<<0)
#define PWR_MGM_CLK_SEL_1	(1<<1)
#define PWR_MGM_CLK_SEL_2	(1<<2)
#define PWR_MGM_STBY_Z	(1<<3)
#define PWR_MGM_STBY_Y	(1<<4)
#define PWR_MGM_STBY_X	(1<<5)
#define PWR_MGM_SLEEP	(1<<6)
#define PWR_MGM_H_RESET	(1<<7)


#define INT_CFG_ACTL	(1<<7)
#define INT_CFG_OPEN	(1<<6)
#define INT_CFG_LATCH_INT_EN	(1<<5)
#define INT_CFG_INT_ANYRD	(1<<4)
#define INT_CFG_ITG_RDY_EN	(1<<2)
#define INT_CFG_RAW_RDY_EN	(1<<0)

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1


// data structures

typedef struct
{
    int8_t h;
    int8_t l;
}data;

typedef struct
{
	data x;
	data y;
	data z;
}adxl345_union

typedef struct
{
	data x;
	data y;
	data z;
	data temp;
}itg3200_union

//#define MS5803_DATA_SIZE (sizeof(ms5803_data)/sizeof(uint8_t))

void adxl337_init();
adxl345_union adxl337_sample();

void print_adxl337_sample(adxl345_union samples);

void itg3200_init();
itg3200_union itg3200_sample();

void print_itg3200_sample(itg3200_union samples);

#endif
