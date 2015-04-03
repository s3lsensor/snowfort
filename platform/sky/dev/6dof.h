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

 * usage: 2 sensors with separate sample and init function
 */
#ifndef _6DOF_H
#define _6DOF_H

//ADXL345 is the accelerometer on 6dof
//ADXL345 Register Map
#define _6DOF_ADXL_ADDR			0xA6
#define _6DOF_DEVID				0x00	//Device ID Register
#define _6DOF_THRESH_TAP		0x1D	//Tap Threshold
#define _6DOF_OFSX				0x1E	//X-axis offset
#define _6DOF_OFSY				0x1F	//Y-axis offset
#define _6DOF_OFSZ				0x20	//Z-axis offset
#define _6DOF_DUR				0x21	//Tap Duration
#define _6DOF_Latent			0x22	//Tap latency
#define _6DOF_Window			0x23	//Tap window
#define _6DOF_THRESH_ACT		0x24	//Activity Threshold
#define _6DOF_THRESH_INACT		0x25	//Inactivity Threshold
#define _6DOF_TIME_INACT		0x26	//Inactivity Time
#define _6DOF_ACT_INACT_CTL		0x27	//Axis enable control for activity and inactivity detection
#define _6DOF_THRESH_FF			0x28	//free-fall threshold
#define _6DOF_TIME_FF			0x29	//Free-Fall Time
#define _6DOF_TAP_AXES			0x2A	//Axis control for tap/double tap
#define _6DOF_ACT_TAP_STATUS	0x2B	//Source of tap/double tap
#define _6DOF_BW_RATE			0x2C	//Data rate and power mode control
#define _6DOF_POWER_CTL			0x2D	//Power Control Register
#define _6DOF_INT_ENABLE		0x2E	//Interrupt Enable Control
#define _6DOF_INT_MAP			0x2F	//Interrupt Mapping Control
#define _6DOF_INT_SOURCE		0x30	//Source of interrupts
#define _6DOF_DATA_FORMAT		0x31	//Data format control
#define _6DOF_DATAX0			0x32	//X-Axis Data 0
#define _6DOF_DATAX1			0x33	//X-Axis Data 1
#define _6DOF_DATAY0			0x34	//Y-Axis Data 0
#define _6DOF_DATAY1			0x35	//Y-Axis Data 1
#define _6DOF_DATAZ0			0x36	//Z-Axis Data 0
#define _6DOF_DATAZ1			0x37	//Z-Axis Data 1
#define _6DOF_FIFO_CTL			0x38	//FIFO control
#define _6DOF_FIFO_STATUS		0x39	//FIFO status

//Power Control Register Bits
#define _6DOF_WU_0			(1<<0)	//Wake Up Mode - Bit 0
#define _6DOF_WU_1			(1<<1)	//Wake Up mode - Bit 1
#define _6DOF_SLEEP			(1<<2)	//Sleep Mode
#define _6DOF_MEASURE		(1<<3)	//Measurement Mode
#define _6DOF_AUTO_SLP		(1<<4)	//Auto Sleep Mode bit
#define _6DOF_LINK			(1<<5)	//Link bit




//ITG3200 is the gyro on 6dof
//ITG3200 Register Map
#define _6DOF_ITG_ADDR		0xD0 //AD0 is tied to gnd
#define _6DOF_WHO_AM_I		0x00
#define _6DOF_SMPLRT_DIV	0x15
#define _6DOF_DLPF_FS		0x16
#define _6DOF_INT_CFG		0x17
#define _6DOF_INT_STATUS	0x1A
#define _6DOF_TEMP_OUT_H	0x1B
#define _6DOF_TEMP_OUT_L	0x1C
#define _6DOF_GYRO_XOUT_H	0x1D
#define _6DOF_GYRO_XOUT_L	0x1E
#define _6DOF_GYRO_YOUT_H	0x1F
#define _6DOF_GYRO_YOUT_L	0x20
#define _6DOF_GYRO_ZOUT_H	0x21
#define _6DOF_GYRO_ZOUT_L	0x22
#define _6DOF_PWR_MGM		0x3E


#define _6DOF_DLPF_CFG_0	(1<<0)
#define _6DOF_DLPF_CFG_1	(1<<1)
#define _6DOF_DLPF_CFG_2	(1<<2)
#define _6DOF_DLPF_FS_SEL_0	(1<<3)
#define _6DOF_DLPF_FS_SEL_1	(1<<4)


#define _6DOF_PWR_MGM_CLK_SEL_0	(1<<0)
#define _6DOF_PWR_MGM_CLK_SEL_1	(1<<1)
#define _6DOF_PWR_MGM_CLK_SEL_2	(1<<2)
#define _6DOF_PWR_MGM_STBY_Z	(1<<3)
#define _6DOF_PWR_MGM_STBY_Y	(1<<4)
#define _6DOF_PWR_MGM_STBY_X	(1<<5)
#define _6DOF_PWR_MGM_SLEEP		(1<<6)
#define _6DOF_PWR_MGM_H_RESET	(1<<7)


#define _6DOF_INT_CFG_ACTL			(1<<7)
#define _6DOF_INT_CFG_OPEN			(1<<6)
#define _6DOF_INT_CFG_LATCH_INT_EN	(1<<5)
#define _6DOF_INT_CFG_INT_ANYRD		(1<<4)
#define _6DOF_INT_CFG_ITG_RDY_EN	(1<<2)
#define _6DOF_INT_CFG_RAW_RDY_EN	(1<<0)

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1


// data structures

typedef struct
{
	int8_t h;
	int8_t l;
}_6dof_data;

typedef struct
{
	_6dof_data x;
	_6dof_data y;
	_6dof_data z;
}adxl345_union;

typedef struct
{
	_6dof_data x;
	_6dof_data y;
	_6dof_data z;
	_6dof_data temp;
}itg3200_union;

//#define MS5803_DATA_SIZE (sizeof(ms5803_data)/sizeof(uint8_t))

void adxl345_enable(void);
adxl345_union adxl345_sample(void);

void print_adxl345_sample(adxl345_union samples);

void itg3200_enable(void);
itg3200_union itg3200_sample(void);

void print_itg3200_sample(itg3200_union samples);

#endif
