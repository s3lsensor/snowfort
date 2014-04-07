/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#ifndef MPU_6050_H
#define MPU_6050_H

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
typedef struct
{
    /* data */
    uint8_t x_accel_h;
    uint8_t x_accel_l;
    uint8_t y_accel_h;
    uint8_t y_accel_l;
    uint8_t z_accel_h;
    uint8_t z_accel_l;
    uint8_t t_h;
    uint8_t t_l;
    uint8_t x_gyro_h;
    uint8_t x_gyro_l;
    uint8_t y_gyro_h;
    uint8_t y_gyro_l;
    uint8_t z_gyro_h;
    uint8_t z_gyro_l;
}mpu_lh;
typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t temperature;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
}mpu_data;

typedef union
{
    mpu_lh reg;
    mpu_data data; 
}mpu_data_union;
int read_mpu_reg(unsigned char mpu_reg_addr, unsigned char* buffer);
int read_mpu_reg_burst(unsigned char mpu_reg_addr_start, unsigned num, unsigned char* buffer);
int write_mpu_reg(unsigned char mpu_reg_addr,unsigned char mpu_reg_val);
int mpu_sample_all(mpu_data_union *sampled_data);
int mpu_reset(void);
int mpu_enable(void);
int mpu_wakeup(void);
int mpu_sleep(void);

#endif /*MPU_6050_H*/
