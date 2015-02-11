# Simple DPDK examples and a UDP packet generator

Simple DPDK examples are mainly based on DPDK examples-l2fwd.

--mirror.c: receives packets and then immediately sends them out again

--multimirror.c same as above, but uses multiple threads

--switch.c: Has two threads that each receive packets on a different NIC port. Packets received in port 1 are sent out port 2 and vice versa

All examples have an optional -v verbose flag which cause them to print out information about each packet received.

go.sh is the script that how to run the examples.

UDP packet generator is mainly based on two web links.

--https://austinmarton.wordpress.com/2011/09/14/sending-raw-ethernet-packets-from-a-specific-interface-in-c-on-linux/

--http://nunix.fr/index.php/programmation/1-c/58-rawsocket-to-forge-udp-packets

--sender.c: takes command line arguments for src/dest: IP, port, and MAC, and a message length. It also needs the ethernet name which the packets will be sent out. It then sends UDP packets to the designated address. The body of the packet can be any data of the specified length. Macro SLEEPUS in sender.h controls the rate. 





