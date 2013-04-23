#!/bin/bash


tail -F log_ap.txt | \
awk \
'{
if($1 == "Beacon") {system("clear"); print op; op = "";} 
op = op "\n" $0;
}'
