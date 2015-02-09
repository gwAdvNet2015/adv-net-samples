CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = -lpcap

all: sniff simple_sniff


simple_sniff: simple_sniff.c
	$(CC) -o $@ $^ $(LDFLAGS)

sniff: simple_sniff2.c
	$(CC) -o $@ $^ $(LDFLAGS)


clean: 
	rm -f simple_sniff sniff
