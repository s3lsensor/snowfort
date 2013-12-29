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
