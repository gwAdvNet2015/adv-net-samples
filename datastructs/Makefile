CC = gcc
CFLAGS = 
DEPS = ring_test.h

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: ring_test

ring_test: ring_test.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm ring_test
