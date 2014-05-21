/*
 * sf_math.c
 *
 *  Created on: Dec 23, 2013
 *      Author: yzliao
 */

/*
 * \file
 *    math library for snowfort
 * \author
 *    Yizheng Liao <yzliao@stanford.edu>
 */

#include <stdlib.h>
#include "sf_math.h"
#include "snowfort.h"


uint8_t sf_math_8bit_abs(const int8_t val)
{
  // from http://graphics.stanford.edu/~seander/bithacks.html
  uint8_t r;
  const int8_t mask = val >> sizeof(int8_t) * CHAR_BIT - 1;

  r = (val ^ mask) - mask;

  return r;
}

/*---------------------------------------------------------------------------*/
uint16_t sf_math_16bit_abs(const int16_t val)
{
  // from http://graphics.stanford.edu/~seander/bithacks.html
  uint16_t r;
  const int16_t mask = val >> sizeof(int16_t) * CHAR_BIT - 1;

  r = (val ^ mask) - mask;

  return r;
}

/*---------------------------------------------------------------------------*/
uint16_t sf_math_16bit_counting_bits(const uint16_t val)
{
    uint16_t count = 0;
    uint16_t v = val;
    while(v)
    {
      count++;
      v >>= 1;
    }

    return count;
}
/*---------------------------------------------------------------------------*/
uint16_t sf_math_8bit_counting_bits(const uint8_t val)
{
  uint16_t val_cast = (uint16_t)val;
  uint16_t count = sf_math_16bit_counting_bits(val_cast);

  return count;
}






