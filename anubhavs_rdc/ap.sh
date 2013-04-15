#!/bin/bash
cp ./tdma_rdc_ap.c ~/contiki/core/net/mac/tdma_rdc.c
make nullApp.upload MOTE=1
make login MOTE=1 | tee log_ap.txt

