all: ringbuffer tests

ringbuffer:
	gcc -c ring.c -o ringbuffer.o
tests:
	cd test && make
clean:
	rm ringbuffer.o
	cd test && make clean
