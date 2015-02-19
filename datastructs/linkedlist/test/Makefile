CC = gcc
CFLAGS = -std=c99 -Wall -pedantic
DEPS = ../linkedlist.o
OBJ = test-linkedlist

all: linkedlist_test

rebuild: clean all

linkedlist_test:
	$(CC) $(CFLAGS) test-ll.c -o $(OBJ) $(DEPS)
clean:
	rm -f $(OBJ)
