#!/bin/bash

source ./test.cfg

if [ "$1" = "make" ]; then
	cp tdmardc.h $CONTIKI_HOME/core/net/mac/tdmardc.h
	cp Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp tdmardc.c $CONTIKI_HOME/core/net/mac/tdmardc.c

	make nullApp.upload MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "test" ]; then
	cp tdmardc.h $CONTIKI_HOME/core/net/mac/tdmardc.h
	cp Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp tdmardc.c $CONTIKI_HOME/core/net/mac/tdmardc.c

	make nullApp.mspsim MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "login" ]; then
	make login MOTE=1 | tee log_ap.txt
fi

