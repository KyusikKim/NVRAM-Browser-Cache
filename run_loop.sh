#!/bin/bash

i=0

while [ ${i} -ne 1 ]
do
	bonnie++ -d ./ -s 8192:2048 -n 10 -m root -r 4096 -u root
done
