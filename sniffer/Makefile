all: sniff


simple_sniff: simple_sniff.c
	gcc -o simple_sniff simple_sniff.c -lpcap

sniff: simple_sniff2.c
	gcc -o sniff simple_sniff2.c -lpcap


clean: 
	rm simple_sniff sniff
