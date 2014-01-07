/*
 * huffman_code.h
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

#ifndef HUFFMAN_CODE_H_
#define HUFFMAN_CODE_H_


/**
 * \brief 8 bits signed integer huffman code encoder
 */
uint16_t huffman_encoder_8bit(int8_t di);

/**
 * \brief 16 bits signed integer huffman code encoder
 */
uint32_t huffman_encoder_16bit(int16_t di);



#endif /* HUFFMAN_CODE_H_ */
