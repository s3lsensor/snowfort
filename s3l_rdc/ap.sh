#!/bin/bash

source ./test.cfg

if [ "$1" = "make" ]; then
	cp tdma-rdc.h $CONTIKI_HOME/core/net/mac/tdma-rdc.h
	cp Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp tdma-rdc.c $CONTIKI_HOME/core/net/mac/tdma-rdc.c

	make nullApp.upload MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "test" ]; then
	cp tdma-rdc.h $CONTIKI_HOME/core/net/mac/tdma-rdc.h
	cp Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp tdma-rdc.c $CONTIKI_HOME/core/net/mac/tdma-rdc.c

	make nullApp.mspsim MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "login" ]; then
	make login MOTE=1 | tee log_ap.txt
fi

