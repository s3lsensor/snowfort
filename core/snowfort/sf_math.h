/**
 * sf_math.h
 * 
 * Created on: Dec 23, 2013
 *      Author: yzliao
 */
 /*
 * \file
 *    math library for snowfort
 * \author
 *    Yizheng Liao <yzliao@stanford.edu>
 */
 #ifndef SF_MATH_H
 #define SF_MATH_H

 /**
 * \brief 8 bits signed number absolute value
 */
uint8_t sf_math_8bit_abs(const int8_t val);

/**
 * \brief 16 bits signed number absolute value
 */
uint16_t sf_math_16bit_abs(const int16_t val);


/**
 * \brief counting number of bits for 16bit integer
 */
uint16_t sf_math_16bit_counting_bits(const uint16_t val);

/**
 * \brief counting number of bits for 8bit integer
 */
uint16_t sf_math_8bit_counting_bits(const uint8_t val);


#endif SF_MATH_H