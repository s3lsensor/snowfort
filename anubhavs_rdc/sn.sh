#!/bin/bash

source ./test.cfg


if [ "$1" = "make" ]; then
	cp ./tdma_rdc_sn.c $CONTIKI_HOME/core/net/mac/tdma_rdc.c
	make nullApp.upload MOTE=$2 BASH_CONST=-DSN_ID=$3
fi

if [ "$1" = "login" ]; then
	make login MOTE=$2 | tee log_sn.txt
fi

