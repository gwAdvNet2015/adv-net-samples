#!/bin/bash

#sudo modprobe uio
#sudo insmod build/kmod/igb_uio.ko

#-- -p PORTMASK  [--config (port,queue,lcore)[,(port,queue,lcore]]
#  -p PORTMASK: hexadecimal bitmask of ports to configure
#  --config (port,queue,lcore): rx queues configuration
#  --no-numa: optional, disable numa awareness

app=$1

if [ -z $app ]
then
	echo "Usage:[mirror|multimirror|switch]"
	exit 1
fi

sudo build/$app -c3 -n4 -- -p 1 -p 3 -v
