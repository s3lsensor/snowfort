#!/bin/bash

source ./test.cfg

if [ "$1" = "make" ]; then
	cp ./tdma_rdc_ap.c $CONTIKI_HOME/core/net/mac/tdma_rdc.c
	make nullApp.upload MOTE=1
fi

if [ "$1" = "login" ]; then
	make login MOTE=1 | tee log_ap.txt
fi

