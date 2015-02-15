#Use the gcc compiler
CC = gcc
CFLAGS = -std=c99 -pedantic -Wall
#List of dependency files needed for compilation
DEPS = hashtable.c
LDFLAGS =
OBJS = hashtable tests

all: $(OBJS)

rebuild: clean all

hashtable:
	$(CC) $(CFLAGS) -c $(DEPS)

tests:
	cd test && make

clean:
	rm -f hashtable.o
	cd test && make clean
