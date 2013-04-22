#!/bin/bash
source ./test.cfg


cp ./tdma_rdc_ap.c $CONTIKI_HOME/core/net/mac/tdma_rdc.c
make nullApp.mspsim

