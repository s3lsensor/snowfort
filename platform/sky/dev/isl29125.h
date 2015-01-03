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

 * usage: 
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

// Configuration Settings
#define CFG_DEFAULT 0x00

// CONFIG1
// Pick a mode, determines what color[s] the sensor samples, if any
#define CFG1_MODE_POWERDOWN 0x00
#define CFG1_MODE_G 0x01
#define CFG1_MODE_R 0x02
#define CFG1_MODE_B 0x03
#define CFG1_MODE_STANDBY 0x04
#define CFG1_MODE_RGB 0x05
#define CFG1_MODE_RG 0x06
#define CFG1_MODE_GB 0x07

// Light intensity range
// In a dark environment 375Lux is best, otherwise 10KLux is likely the best option
#define CFG1_375LUX 0x00
#define CFG1_10KLUX 0x08

// Change this to 12 bit if you want less accuracy, but faster sensor reads
// At default 16 bit, each sensor sample for a given color is about ~100ms
#define CFG1_16BIT 0x00
#define CFG1_12BIT 0x10

// Unless you want the interrupt pin to be an input that triggers sensor sampling, leave this on normal
#define CFG1_ADC_SYNC_NORMAL 0x00
#define CFG1_ADC_SYNC_TO_INT 0x20

// CONFIG2
// Selects upper or lower range of IR filtering
#define CFG2_IR_OFFSET_OFF 0x00
#define CFG2_IR_OFFSET_ON 0x80

// Sets amount of IR filtering, can use these presets or any value between 0x00 and 0x3F
// Consult datasheet for detailed IR filtering calibration
#define CFG2_IR_ADJUST_LOW 0x00
#define CFG2_IR_ADJUST_MID 0x20
#define CFG2_IR_ADJUST_HIGH 0x3F

// CONFIG3
// No interrupts, or interrupts based on a selected color
#define CFG3_NO_INT 0x00
#define CFG3_G_INT 0x01
#define CFG3_R_INT 0x02
#define CFG3_B_INT 0x03

// How many times a sensor sample must hit a threshold before triggering an interrupt
// More consecutive samples means more times between interrupts, but less triggers from short transients
#define CFG3_INT_PRST1 0x00
#define CFG3_INT_PRST2 0x04
#define CFG3_INT_PRST4 0x08
#define CFG3_INT_PRST8 0x0C

// If you would rather have interrupts trigger when a sensor sampling is complete, enable this
// If this is disabled, interrupts are based on comparing sensor data to threshold settings
#define CFG3_RGB_CONV_TO_INT_DISABLE 0x00
#define CFG3_RGB_CONV_TO_INT_ENABLE 0x10

// STATUS FLAG MASKS
#define FLAG_INT 0x01
#define FLAG_CONV_DONE 0x02
#define FLAG_BROWNOUT 0x04
#define FLAG_CONV_G 0x10
#define FLAG_CONV_R 0x20
#define FLAG_CONV_B 0x30



#define SWAP(a,b) a = a^b; b = a^b; a = a^b;
#define MPU_PRINT_BYTE( X ) (uart1_writeb((unsigned char)X));

#define I2C_READ_SEND_ACK 1


// #define MS5803_DATA_SIZE (sizeof(ms5803_data)/sizeof(uint8_t))

void isl29125_init(void);
void isl29125_config(int8_t config1, int8_t config2, int8_t config3);
void setUpperThreshold(int16_t data);
void setLowerThreshold(uint16_t data);
int16_t readUpperThreshold(void);
int16_t readLowerThreshold(void);
int16_t isl29125_sample_red(void);
int16_t isl29125_sample_blue(void);
int16_t isl29125_sample_green(void);
int8_t isl29125_readStatus(void);

void print_isl29125_sample(int16_t data);

#endif
