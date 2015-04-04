/*
 * This file defines the driver for htu21d
 *
 * ---------------------------------------------------------
 *
 * Author: Bin Wang
 */

/*
 * Pin connection: 
		   + -> U2-1
		   - -> U2-9
		   SCL -> U28-3
		   SDA -> U28-4


 * The resolution is set to be 8 bits
 */
#ifndef HTU21D_H
#define HTU21D_H


#define HTU21D_ADDRESS 0x40 << 1 

#define HTU21D_TRIGGER_TEMP_MEASURE_HOLD  0xE3
#define HTU21D_TRIGGER_HUMD_MEASURE_HOLD  0xE5
#define HTU21D_WRITE_USER_REG  0xE6
#define HTU21D_READ_USER_REG  0xE7
#define HTU21D_SOFT_RESET  0xFE



typedef struct
{
    int8_t h;
    int8_t l;
    int8_t crc;
}htu21d_data;

// functions for users
void htu21d_enable(void);
htu21d_data htu21d_sample_hum(void);
htu21d_data htu21d_sample_tmp(void);
void print_htu21d_sample(htu21d_data samples);

// functions for internal use
void htu21d_set_res(unsigned resolution);
#endif
