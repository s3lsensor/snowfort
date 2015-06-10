/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/i2c.h"
#include "dev/mpu-6050.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


// variable to control print
volatile uint8_t print_MPU;

/*
 * Read a single byte from the MPU.
 */
int read_mpu_reg(uint8_t mpu_reg_addr, uint8_t *buffer)
{
	return read_mpu_reg_burst(mpu_reg_addr,1,buffer);

	/*int ret=0;

	i2c_start();

	if(i2c_write(MPU_ADDR_W)){ //device address written successfully, ACK received from MPU
		if(i2c_write(mpu_reg_addr)){//register write successful, ACK from MPU received
			i2c_start();
			if(!i2c_write(MPU_ADDR_R))
				printf("Read_MPU: MPU addr not written to i2c before reading.\n");
			else{
				*buffer=i2c_read(!I2C_READ_SEND_ACK); //only need to send to read in burst mode
				ret=1;
			}
		}
		else
			printf("Read_MPU: Register addr not written to i2c.\n");
	}
	else
		printf("READ_MPU: MPU addr not written to i2c.\n");

	i2c_stop();

	return ret;*/
}


/*
 * Write a value to a MPU register. See datasheet for details
 */
int write_mpu_reg(unsigned char mpu_reg_addr,unsigned char mpu_reg_val)
{
	int ret=0;

	i2c_start();

	if(i2c_write(MPU_ADDR_W)){ //device address written successfully, ACK received from MPU
		if(i2c_write(mpu_reg_addr)){//register write successful, ACK from MPU received
			if(i2c_write(mpu_reg_val))
				ret=1;
			else
				printf("Wrt_MPU: Reg value not written to i2c.\n");
		}
		else
			printf("Wrt_MPU: Reg addr not written to i2c.\n");
	}
	else
		printf("Wrt_MPU: MPU addr not written to i2c.\n");

	i2c_stop();

	return ret;
}

/*
 * Read multiple bytes from the MPU. See datasheet for burst mode read for details
 */
int read_mpu_reg_burst(unsigned char mpu_reg_addr_start, unsigned num, uint8_t *buffer)
{
	int ret=0,i;

	if(num<1)
		return ret;

	i2c_start();

	if(i2c_write(MPU_ADDR_W)){ //device address written successfully, ACK received from MPU
		if(i2c_write(mpu_reg_addr_start)){//register write successful, ACK from MPU received
			i2c_start();
			if(!i2c_write(MPU_ADDR_R))
				printf("Read_MPU_burst: MPU addr not written to i2c before reading.\n");
			else{
				for(i=0;i<num-1;i++)
					*(buffer+i)=i2c_read(I2C_READ_SEND_ACK); //only need to send to read in burst mode
				*(buffer+num-1)=i2c_read(!I2C_READ_SEND_ACK);
				ret=1;
			}
		}
		else
			printf("Read_MPU_burst: Register addr not written to i2c.\n");
	}
	else
		printf("READ_MPU_burst: MPU addr not written to i2c.\n");

	i2c_stop();

	return ret;

}

//Takes about 1.4 ms to finish
int mpu_sample_all(mpu_data_union *sampled_data)
{
	//uint8_t buffer[14];
	if(!read_mpu_reg_burst(MPU_RA_ACCEL_XOUT_H,14,(uint8_t*)sampled_data))
		return 0;

/*

	sampled_data->accel_x = (buffer[0]<<8)+buffer[1];
	sampled_data->accel_y = (buffer[2]<<8)+buffer[3];
	sampled_data->accel_z = (buffer[4]<<8)+buffer[5];
	sampled_data->temperature = (buffer[6]<<6)+buffer[7];
	sampled_data->gyro_x = (buffer[8]<<8)+buffer[9];
	sampled_data->gyro_y = (buffer[10]<<8)+buffer[11];
	sampled_data->gyro_z = (buffer[12]<<8)+buffer[13];
*/
	SWAP(sampled_data->reg.x_accel_h,sampled_data->reg.x_accel_l);
	SWAP(sampled_data->reg.y_accel_h,sampled_data->reg.y_accel_l);
	SWAP(sampled_data->reg.z_accel_h,sampled_data->reg.z_accel_l);
	SWAP(sampled_data->reg.t_h,sampled_data->reg.t_l);
	SWAP(sampled_data->reg.x_gyro_h,sampled_data->reg.x_gyro_l);
	SWAP(sampled_data->reg.y_gyro_h,sampled_data->reg.y_gyro_l);
	SWAP(sampled_data->reg.z_gyro_h,sampled_data->reg.z_gyro_l);


	PRINTF("%d,%d,%d,%d,%d,%d,%d\n",sampled_data->data.accel_x,sampled_data->data.accel_y,sampled_data->data.accel_z,sampled_data->data.gyro_x,sampled_data->data.gyro_y,
		sampled_data->data.gyro_z,sampled_data->data.temperature);

	return 1;
}

int mpu_sample_acc(mpu_data_acc_gyro_union *sampled_data)
{
	///uint8_t buffer[6];
	if(!read_mpu_reg_burst(MPU_RA_ACCEL_XOUT_H,6,(uint8_t*)sampled_data))
		return 0;

	SWAP(sampled_data->reg.x_h,sampled_data->reg.x_l);
	SWAP(sampled_data->reg.y_h,sampled_data->reg.y_l);
	SWAP(sampled_data->reg.z_h,sampled_data->reg.z_l);

	// sampled_data->reg.x_h = buffer[0];
	// sampled_data->reg.x_l = buffer[1];
	// sampled_data->reg.y_h = buffer[2];
	// sampled_data->reg.y_l = buffer[3];
	// sampled_data->reg.z_h = buffer[4];
	// sampled_data->reg.z_l = buffer[5];

	return 1;
}

int mpu_sample_gyro(mpu_data_acc_gyro_union *sampled_data)
{
	//uint8_t buffer[6];
	if(!read_mpu_reg_burst(MPU_RA_GYRO_XOUT_H,6,(uint8_t*)sampled_data))
		return 0;

	SWAP(sampled_data->reg.x_h,sampled_data->reg.x_l);
	SWAP(sampled_data->reg.y_h,sampled_data->reg.y_l);
	SWAP(sampled_data->reg.z_h,sampled_data->reg.z_l);

	return 1;

}

/*
Restore the default configuration (default register values)
*/
int mpu_reset(void)
{
	int ret=0;

	if(write_mpu_reg(MPU_RA_PWR_MGMT1,MPU_RESET_BIT)){
		clock_delay(10000);
		ret=1;
	}
/*
	write_(MPU_ADDR_W,MPU_RA_PWR_MGMT1,MPU_RESET_BIT);
	ret = 1;
*/

	return ret;
}

/*
 * Resets the MPU and enables I2C communication with it
 */
int mpu_enable(void)
{
	int ret=0;

	i2c_enable();
	if(!mpu_reset())
		  printf("Could not reset the MPU");
	else
		ret=1;

	return ret;
}

/*
 * Wake up MPU from sleep (default) mode and turn sensors on
 */
int mpu_wakeup(void)
{
	uint8_t reg_val=0x02; //any number other than 0x40 and 0x00
	int ret=0;

	write_mpu_reg(MPU_RA_PWR_MGMT1,MPU_RV_PWR_MGMT1_AWAKE);
	read_mpu_reg(MPU_RA_PWR_MGMT1,&reg_val);

	if(reg_val==MPU_RV_PWR_MGMT1_AWAKE){
		printf("MPU awake and all sensors ON.\n");
		ret=1;
	}
	else
		printf("Couldn't turn on MPU sensors.\n",reg_val);

	return ret;
}

/*
 * Put the MPU to sleep and turn off all sensors.
 * After reset, the default state is sleep.
 */
int mpu_sleep(void)
{
	mpu_reset();
}

/*
 * Print the MPU samples
 */
void print_mpu_sample(mpu_data_union *samples)
{
	uart1_writeb((unsigned char)samples->reg.x_accel_h);
	uart1_writeb((unsigned char)samples->reg.x_accel_l);
	uart1_writeb((unsigned char)samples->reg.y_accel_h);
	uart1_writeb((unsigned char)samples->reg.y_accel_l);
	uart1_writeb((unsigned char)samples->reg.z_accel_h);
	uart1_writeb((unsigned char)samples->reg.z_accel_l);
	uart1_writeb((unsigned char)samples->reg.x_gyro_h);
	uart1_writeb((unsigned char)samples->reg.x_gyro_l);
	uart1_writeb((unsigned char)samples->reg.y_gyro_h);
	uart1_writeb((unsigned char)samples->reg.y_gyro_l);
	uart1_writeb((unsigned char)samples->reg.z_gyro_h);
	uart1_writeb((unsigned char)samples->reg.z_gyro_l);
	uart1_writeb((unsigned char)samples->reg.t_h);
	uart1_writeb((unsigned char)samples->reg.t_l);
}


void print_mpu_sample_acc_gyro(mpu_data_acc_gyro_union *samples)
{
	uart1_writeb((unsigned char)samples->reg.x_h);
	uart1_writeb((unsigned char)samples->reg.x_l);
	uart1_writeb((unsigned char)samples->reg.y_h);
	uart1_writeb((unsigned char)samples->reg.y_l);
	uart1_writeb((unsigned char)samples->reg.z_h);
	uart1_writeb((unsigned char)samples->reg.z_l);
}


void mpu_get_acc(mpu_data_union *sampled_data,mpu_data_acc_gyro_union *acc_sample)
{

	acc_sample->data.x = sampled_data->data.accel_x;
	acc_sample->data.y = sampled_data->data.accel_y;
	acc_sample->data.z = sampled_data->data.accel_z;

}

void mpu_get_gyro(mpu_data_union *sampled_data,mpu_data_acc_gyro_union *gyro_sample)
{

	gyro_sample->data.x = sampled_data->data.gyro_x;
	gyro_sample->data.y = sampled_data->data.gyro_y;
	gyro_sample->data.z = sampled_data->data.gyro_z;

}

void mpu_get_tp(mpu_data_union *sampled_data,mpu_data_tp_union *tp_sample)
{
	tp_sample->data = sampled_data->data.tp;
}




