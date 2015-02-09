all: hashtable_test

hashtable_test:
	gcc -std=c99 -pedantic -Wall test-hashtable.c -o test-hashtable ../hashtable.o ../../linkedlist/linkedlist.o

clean:
	rm test-hashtable
