CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
DEPS = ../hashtable.o ../../linkedlist/linkedlist.o
OBJ = test-hashtable

all: hashtable_test

rebuild: clean all

hashtable_test:
	$(CC) $(CFLAGS) -o $(OBJ) $(DEPS)
clean:
	rm -f $(OBJ)
