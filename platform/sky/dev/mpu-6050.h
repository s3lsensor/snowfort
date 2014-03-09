/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */


#define I2C_READ_SEND_ACK 1



#define MPU_ADDR 0x68
#define MPU_ADDR_W (MPU_ADDR<<1)
#define MPU_ADDR_R ((MPU_ADDR<<1)+1)
#define MPU_SLEEP_BIT BV(6)
#define MPU_RESET_BIT BV(7)


#define MPU_RA_ACCEL_XOUT_H 0x3b
#define MPU_RA_ACCEL_XOUT_L 0x3c
#define MPU_RA_ACCEL_YOUT_H 0x3d
#define MPU_RA_ACCEL_YOUT_L 0x3e
#define MPU_RA_ACCEL_ZOUT_H 0x3f
#define MPU_RA_ACCEL_ZOUT_L 0x40
#define MPU_RA_TEMP_H 0x41
#define MPU_RA_TEMP_L 0x42
#define MPU_RA_GYRO_XOUT_H 0x43
#define MPU_RA_GYRO_XOUT_L 0x44
#define MPU_RA_GYRO_YOUT_H 0x45
#define MPU_RA_GYRO_YOUT_L 0x46
#define MPU_RA_GYRO_ZOUT_H 0x47
#define MPU_RA_GYRO_ZOUT_L 0x48


#define MPU_RA_WHO_AM_I 0x75
#define MPU_RA_PWR_MGMT1 0x6b
#define MPU_RV_PWR_MGMT1_AWAKE 0x00
#define MPU_RA_PWR_MGMT2 0x6c

struct mpu_data{
	short accel_x;
	short accel_y;
	short accel_z;
	short temperature;
	short gyro_x;
	short gyro_y;
	short gyro_z;
};

int read_mpu_reg(unsigned char mpu_reg_addr, unsigned char* buffer);
int read_mpu_reg_burst(unsigned char mpu_reg_addr_start, unsigned num, unsigned char* buffer);
int write_mpu_reg(unsigned char mpu_reg_addr,unsigned char mpu_reg_val);
int mpu_sample_all(struct mpu_data * sampled_data);
int mpu_reset(void);
int mpu_enable(void);
int mpu_wakeup(void);
int mpu_sleep(void);

