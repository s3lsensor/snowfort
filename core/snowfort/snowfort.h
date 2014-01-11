/*
 * snowfort.h
 *
 *  Created on: Jan 10th, 2014
 *      Author: yzliao
 */

/*
 * \file
 *    header file for snowfort
 * \author
 *    Yizheng Liao <yzliao@stanford.edu>
 */

 /**
  * Define constant
  */

// the number of bits per byte
#define CHAR_BIT 8

/**
 * \brief structure for bit
 */
typedef struct 
{
	/* data */
	uint8_t bit : 1;
}bit_t;

/**
 * \brief union of 8 bits integer
 */
typedef union
{
	bit_t 		bits[8];
	uint8_t 	int_val; 
}bit8_t;


/**
 * \brief union of 16 bits integer
 */
typedef union
{
	bit_t 		bits[16];
	uint16_t    int_val;
}bit16_t;


/**
 * \brief union of 32 bits integer
 */
typedef union
{
	bit_t 		bits[32];
	uint32_t 	int_val;
}bit32_t;