/*
 * This file defines the driver for 6dof
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang 
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/i2c.h"
#include "dev/6dof.h"
#include "dev/uart1.h"

// Initialize the sensor and return coefficients
void adxl345_enable(void){
	i2c_enable();
	write_(_6DOF_ADXL_ADDR, _6DOF_POWER_CTL, _6DOF_MEASURE);
	write_(_6DOF_ADXL_ADDR, _6DOF_DATA_FORMAT, 1<<0);
}

adxl345_union adxl345_sample(void){
	_6dof_data tmp;
	adxl345_union rlt;

	tmp.h = read_(_6DOF_ADXL_ADDR, _6DOF_DATAX0, 0);
	tmp.l = read_(_6DOF_ADXL_ADDR, _6DOF_DATAX1, 0);
	rlt.x = tmp;

	tmp.h = read_(_6DOF_ADXL_ADDR, _6DOF_DATAY0, 0);
	tmp.l = read_(_6DOF_ADXL_ADDR, _6DOF_DATAY1, 0);
	rlt.y = tmp;

	tmp.h = read_(_6DOF_ADXL_ADDR, _6DOF_DATAZ0, 0);
	tmp.l = read_(_6DOF_ADXL_ADDR, _6DOF_DATAZ1, 0);
	rlt.z = tmp;

	return rlt;

}

void print_adxl345_sample(adxl345_union samples){

	uart1_writeb((unsigned char) samples.x.h);
	uart1_writeb((unsigned char) samples.x.l);

	uart1_writeb((unsigned char) samples.y.h);
	uart1_writeb((unsigned char) samples.y.l);

	uart1_writeb((unsigned char) samples.z.h);
	uart1_writeb((unsigned char) samples.z.l);

}

void itg3200_enable(void){
	i2c_enable();

	//Set internal clock to 1kHz with 42Hz LPF and Full Scale to 3 for proper operation
	write_(_6DOF_ITG_ADDR, _6DOF_DLPF_FS, _6DOF_DLPF_FS_SEL_0|_6DOF_DLPF_FS_SEL_1|_6DOF_DLPF_CFG_0);
	
	//Set sample rate divider for 100 Hz operation
	write_(_6DOF_ITG_ADDR, _6DOF_SMPLRT_DIV, 9);	//Fsample = Fint / (divider + 1) where Fint is 1kHz
	
	//Setup the interrupt to trigger when new data is ready.
	write_(_6DOF_ITG_ADDR,  _6DOF_INT_CFG, _6DOF_INT_CFG_RAW_RDY_EN | _6DOF_INT_CFG_ITG_RDY_EN);
	
	//Select X gyro PLL for clock source
	write_(_6DOF_ITG_ADDR, _6DOF_PWR_MGM, _6DOF_PWR_MGM_CLK_SEL_0);
}


itg3200_union itg3200_sample(void){
	_6dof_data tmp;
	itg3200_union rlt;

	tmp.h = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_XOUT_H, 0);
	tmp.l = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_XOUT_L, 0);
	rlt.x = tmp;

	tmp.h = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_YOUT_H, 0);
	tmp.l = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_YOUT_L, 0);
	rlt.y = tmp;

	tmp.h = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_ZOUT_H, 0);
	tmp.l = read_(_6DOF_ITG_ADDR, _6DOF_GYRO_ZOUT_L, 0);
	rlt.z = tmp;


	tmp.h = read_(_6DOF_ITG_ADDR, _6DOF_TEMP_OUT_H, 0);
	tmp.l = read_(_6DOF_ITG_ADDR, _6DOF_TEMP_OUT_L, 0);
	rlt.temp = tmp;
	return rlt;
}

void print_itg3200_sample(itg3200_union samples){
	uart1_writeb((unsigned char) samples.x.h);
	uart1_writeb((unsigned char) samples.x.l);

	uart1_writeb((unsigned char) samples.y.h);
	uart1_writeb((unsigned char) samples.y.l);

	uart1_writeb((unsigned char) samples.z.h);
	uart1_writeb((unsigned char) samples.z.l);

	uart1_writeb((unsigned char) samples.temp.h);
	uart1_writeb((unsigned char) samples.temp.l);
}

