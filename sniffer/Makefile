CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = -lpcap

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: sniff simple_sniff

rebuild: clean all

simple_sniff: simple_sniff.c
	$(CC) -o $@ $^ $(LDFLAGS)

sniff: simple_sniff2.c
	$(CC) -o $@ $^ $(LDFLAGS)


clean: 
	@rm -f simple_sniff sniff
