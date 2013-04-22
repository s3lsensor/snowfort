#!/bin/bash

source ./test.cfg

cp ./tdma_rdc_sn.c $CONTIKI_HOME/core/net/mac/tdma_rdc.c
make nullApp.mspsim BASH_CONST=-DSN_ID=23
