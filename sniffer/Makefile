all: sniff


simple_sniff: simple_sniff.c
	gcc -o simple_sniff simple_sniff.c

sniff: simple_sniff2.c
	gcc -o sniff simple_sniff2.c


clean: 
	rm simple_sniff sniff
