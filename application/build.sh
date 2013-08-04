#!/bin/bash

source ./test.cfg

if [ "$1" = "make" ]; then
	cp $CONTIKI_HOME/s3l_rdc/tdmardc.h $CONTIKI_HOME/core/net/mac/tdmardc.h
	cp $CONTIKI_HOME/s3l_rdc/Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp $CONTIKI_HOME/s3l_rdc/tdmardc.c $CONTIKI_HOME/core/net/mac/tdmardc.c
	cp $CONTIKI_HOME/s3l_rdc/i2c.c $CONTIKI_HOME/platform/sky/dev/i2c.c
	cp $CONTIKI_HOME/s3l_rdc/i2c.h $CONTIKI_HOME/platform/sky/dev/i2c.h

	make nullApp.upload MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "test" ]; then
	cp $CONTIKI_HOME/s3l_rdc/tdmardc.h $CONTIKI_HOME/core/net/mac/tdmardc.h
	cp $CONTIKI_HOME/s3l_rdc/Makefile.mac $CONTIKI_HOME/core/net/mac/Makefile.mac
	cp $CONTIKI_HOME/s3l_rdc/tdmardc.c $CONTIKI_HOME/core/net/mac/tdmardc.c
	cp $CONTIKI_HOME/s3l_rdc/i2c.c $CONTIKI_HOME/platform/sky/dev/i2c.c
	cp $CONTIKI_HOME/s3l_rdc/i2c.h $CONTIKI_HOME/platform/sky/dev/i2c.h

	make nullApp.mspsim MOTE=1 BASH_CONST=-DSN_ID=$2
fi

if [ "$1" = "login" ]; then
	make login MOTE=1 | tee log_ap.txt
fi

