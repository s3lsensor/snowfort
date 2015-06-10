/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#ifndef MPU_6050_H
#define MPU_6050_H

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1




#define MPU_ADDR 0x68
#define MPU_ADDR_W (MPU_ADDR<<1)
#define MPU_ADDR_R ((MPU_ADDR<<1)+1)
#define MPU_SLEEP_BIT BV(6)
#define MPU_RESET_BIT BV(7)

#define MPU6050_RA_INT_STATUS       0x3A

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

#define MPU_SMPLRT_DIV             0x19
#define MPU_CONFIG                 0x1a
#define MPU_GYRO_CONFIG            0x1b
#define MPU_ACCEL_CONFIG           0x1c



typedef struct
{
    /* data */
    int8_t x_accel_h;
    int8_t x_accel_l;
    int8_t y_accel_h;
    int8_t y_accel_l;
    int8_t z_accel_h;
    int8_t z_accel_l;
    int8_t t_h;
    int8_t t_l;
    int8_t x_gyro_h;
    int8_t x_gyro_l;
    int8_t y_gyro_h;
    int8_t y_gyro_l;
    int8_t z_gyro_h;
    int8_t z_gyro_l;
}mpu_lh;

typedef struct 
{
    /* data */
    int8_t x_h;
    int8_t x_l;
    int8_t y_h;
    int8_t y_l;
    int8_t z_h;
    int8_t z_l;

}mpu_lh_acc_gyro;

typedef struct 
{
    /* data */
    int8_t t_h;
    int8_t t_l;
}mpu_lh_tp;

typedef struct
{
    int16_t accel_x;
    int16_t accel_y;
    int16_t accel_z;
    int16_t tp;
    int16_t gyro_x;
    int16_t gyro_y;
    int16_t gyro_z;
}mpu_data;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
}mpu_data_acc_gyro;


typedef union
{
    mpu_lh reg;
    mpu_data data;
}mpu_data_union;

typedef union
{
    mpu_lh_acc_gyro reg;
    mpu_data_acc_gyro data;
}mpu_data_acc_gyro_union;

typedef union
{
    mpu_lh_tp reg;
    int16_t data;
}mpu_data_tp_union;

#define MPU_DATA_SIZE (sizeof(mpu_data)/sizeof(uint8_t))

#define MPU_DATA_ACC_GYRO_SIZE (sizeof(mpu_data_acc_gyro)/sizeof(uint8_t))

#define MPU_DATA_TP_SIZE (sizeof(int16_t)/sizeof(uint8_t))

int read_mpu_reg(unsigned char mpu_reg_addr, unsigned char* buffer);
int read_mpu_reg_burst(unsigned char mpu_reg_addr_start, unsigned num, unsigned char* buffer);
int write_mpu_reg(unsigned char mpu_reg_addr,unsigned char mpu_reg_val);
int mpu_sample_all(mpu_data_union *sampled_data);
int mpu_sample_acc(mpu_data_acc_gyro_union *sampled_data);
int mpu_sample_gyro(mpu_data_acc_gyro_union *sampled_data);
int mpu_reset(void);
int mpu_enable(void);
int mpu_wakeup(void);
int mpu_sleep(void);

void print_mpu_sample(mpu_data_union *samples);
void print_mpu_sample_acc_gyro(mpu_data_acc_gyro_union *samples);

void mpu_get_acc(mpu_data_union *sampled_data,mpu_data_acc_gyro_union *acc_sample);
void mpu_get_gyro(mpu_data_union *sampled_data,mpu_data_acc_gyro_union *gyro_sample);

void mpu_get_tp(mpu_data_union *sampled_data,mpu_data_tp_union *tp_sample);



// variable to control print
extern volatile uint8_t print_MPU;

#endif /*MPU_6050_H*/
