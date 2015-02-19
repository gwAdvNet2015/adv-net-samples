CC = gcc
DEPS = ring_test.c


all: ringbuffer_test

rebuild: clean all

ringbuffer_test:
	$(CC) $(DEPS) -o test-ringbuffer ../ringbuffer.o

clean:
	rm -f test-ringbuffer
