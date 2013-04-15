#!/bin/bash
cp ./tdma_rdc_sn.c ~/contiki/core/net/mac/tdma_rdc.c
make nullApp.upload MOTE=2 BASH_CONST=-DSN_ID=$1
make login MOTE=2 | tee log_sn.txt

