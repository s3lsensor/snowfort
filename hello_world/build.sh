#!/bin/bash

MOTE_ID=$2
: ${MOTE_ID=1}

if [ "$1" = "make" ]; then
	make clean
	make sensors="$(echo $(tr '[:upper:]' '[:lower:]' <<< "${@:5}") | sed 's/ /.c /g').c" $3.upload MOTE=$MOTE_ID BASH_CONST=-DSN_ID=$4
fi

if [ "$1" = "test" ]; then
    make clean
	make $3.mspsim MOTE=$MOTE_ID BASH_CONST=-DSN_ID=$4
fi

if [ "$1" = "login" ]; then
	make login MOTE=$MOTE_ID
fi

if [ "$1" = "clean" ]; then
    rm *.sky
    make clean
fi
