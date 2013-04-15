#!/bin/bash
cp ./tdma_rdc_sn.c ~/contiki/core/net/mac/tdma_rdc.c
make nullApp.mspsim BASH_CONST=-DSN_ID=23
