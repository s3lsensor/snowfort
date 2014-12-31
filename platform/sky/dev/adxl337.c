/*
 * This file defines the driver for mpu-6050
 *
 * ---------------------------------------------------------
 *
 * Author: Ronnie Bajwa
 */
#include "contiki.h"

#include <stdio.h>

#include "dev/i2c.h"
#include "dev/ml8511.h"
#include "dev/uart1.h"

#define SWAP(a,b) a = a^b; b = a^b; a = a^b;

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif


/*
 * Read a single byte from the MPU.
 */

// dummy file for adxl337, analog output need an adc to continue



