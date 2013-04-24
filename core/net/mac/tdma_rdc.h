/*
 * This file is a TDMA RDC protocol. It is based on the Contiki operating system.
 *
 * $Id: tdma_rdc.h,v 1.0 2013/04/22 20:00:00 nifi Exp $
 */

/**
 * \file
 *         A TDMA RDC implementation that uses framer for headers.
 * \author
 *         Yizheng Liao <yzliao@stanford.edu>
 */

#ifndef __TDMA_RDC_H__
#define __TDMA_RDC_H__

#include "net/mac/rdc.h"
#include "dev/radio.h"

extern const struct rdc_driver tdma_rdc_driver;

#endif /* __TDMA_RDC_H__ */
