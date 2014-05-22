#!/bin/bash

MOTE_ID=$4
: ${MOTE_ID=1}

if [ "$1" = "make" ]; then
    make clean
	make $2.upload MOTE=$MOTE_ID BASH_CONST=-DSN_ID=$3
fi

if [ "$1" = "test" ]; then
    make clean
	make $2.mspsim MOTE=$MOTE_ID BASH_CONST=-DSN_ID=$3
fi

if [ "$1" = "login" ]; then
	make login MOTE=$MOTE_ID | tee log_ap.txt
fi

if [ "$1" = "clean" ]; then
    rm *.sky
    make clean
fi
