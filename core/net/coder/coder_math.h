/*
 * coder_math.h
 *
 *  Created on: Dec 23, 2013
 *      Author: yzliao
 */
/*
 * \file
 *    math library for code scheme
 * \author
 *    Yizheng Liao <yzliao@stanford.edu>
 */
#ifndef CODER_MATH_H_
#define CODER_MATH_H_

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

/**
 * \brief 8 bits signed number absolute value
 */
uint8_t sf_coder_math_8bit_abs(const int8_t val);

/**
 * \brief 16 bits signed number absolute value
 */
uint16_t sf_coder_math_16bit_abs(const int16_t val);


/**
 * \brief counting number of bits for 16bit integer
 */
uint16_t sf_coder_math_16bit_counting_bits(const uint16_t val);

/**
 * \brief counting number of bits for 8bit integer
 */
uint16_t sf_coder_math_8bit_counting_bits(const uint8_t val);

#endif /* CODER_MATH_H_ */
