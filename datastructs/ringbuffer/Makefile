CC = gcc
CFLAGS =
DEPS = ring.c
OBJ = ringbuffer

all: ringbuffer tests

rebuild: clean all

ringbuffer:
	$(CC) -c $(DEPS) -o ringbuffer.o
tests:
	cd test && make
clean:
	rm ringbuffer.o
	cd test && make clean
