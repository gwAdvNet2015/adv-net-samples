
all: hashtable tests

hashtable:
	gcc -std=c99 -pedantic -Wall -c hashtable.c

tests:
	cd test && make

clean:
	rm hashtable.o
	cd test && make clean
