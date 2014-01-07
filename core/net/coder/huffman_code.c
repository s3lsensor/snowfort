/*
 * huffman_code.c
 *
 *  Created on: Dec 23, 2013
 *      Author: yzliao
 */

/*
 * \file
 *    Huffman code scheme
 * \author
 *    Yizheng Liao <yzliao@stanford.edu>
 */

#include <stdlib.h>
#include "huffman_code.h"
#include "coder_math.h"

#define HUFFMAN_CODE_HEADER_LEN 16

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

// huffman code header
static const uint8_t huffman_code_header[HUFFMAN_CODE_HEADER_LEN] = {
    0,  // 0000b
    1,  // 0001b
    2,  // 0010b
    3,  // 0011b
    4,  // 0100b
    5,  // 0101b
    6,  // 0110b
    7,  // 0111b
    8,  // 1000b
    9,  // 1001b
    10, // 1010b
    11, // 1011b
    12, // 1100b
    13, // 1101b
    14, // 1110b
    15  // 1111b
};

/*---------------------------------------------------------------------------*/
uint16_t huffman_encoder_8bit(int8_t di)
{
  uint8_t ni;
  uint8_t si;
  uint8_t ai;
  uint16_t bsi;  //max size is 4+8=12 bits
  uint8_t di_abs;
  uint8_t ai_mask;

  if (di == 0)
  {
    ni = 0;
  }
  else
  {
    di_abs = sf_coder_math_8bit_abs(di);
    ni = sf_coder_math_8bit_counting_bits(di_abs);
  }

  PRINTF("di %d, di_abs %d, ni %u\n",di,di_abs,ni);
  si = huffman_code_header[ni];

  if(ni == 0)
  {
    bsi = (uint16_t)si;
  }
  else
  {
    if(di > 0)
    {
      ai_mask = (1 << ni) - 1;
      ai = di & ai_mask;
    }
    else // di < 0 (no di == 0 case)
    {
      ai_mask = (1 << ni) - 1;
      ai = (di-1) & ai_mask;
    }

    //set bsi
    //bsi = (si << ni) | ai;
    bsi = (uint16_t)si;
    bsi = (bsi << ni) | ai;
  }

  PRINTF("si 0x%x, ai 0x%x, bsi 0x%x\n",si,ai,bsi);

  return bsi;


}

/*---------------------------------------------------------------------------*/
uint32_t huffman_encoder_16bit(int16_t di)
{
    uint8_t ni;
    uint16_t si;
    uint16_t ai;
    uint32_t bsi = 0;
    uint16_t di_abs;
    uint16_t ai_mask;

    if (di == 0)
    {
      ni = 0;
    }
    else
    {
      di_abs = sf_coder_math_16bit_abs(di);
      ni = sf_coder_math_16bit_counting_bits(di_abs);
    }

    PRINTF("di %d, di_abs %d, ni %u\n",di,di_abs,ni);

    si = huffman_code_header[ni];

    if (ni == 0)
    {
      bsi = (uint32_t)si;
    }
    else
    {
      if (di > 0)
      {
        ai_mask = (1 << ni) - 1;
        ai = di & ai_mask;
      }
      else
      {
        ai_mask = (1 << ni) - 1;
        ai = (di - 1) & ai_mask;
      }
    }

    // set bsi
    bsi = (uint32_t)si;
    bsi = (bsi << ni) | ai;

    PRINTF("si 0x%x, ai 0x%x, bsi 0x%x\n",si,ai,bsi);

    return bsi;
}




