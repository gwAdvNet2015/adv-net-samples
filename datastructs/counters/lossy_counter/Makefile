#Use the gcc compiler
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall
RM=rm

#List of dependency files needed for compilation
DEPS=zipfgen.c lossycon.c lossy_test.c
LDFLAGS=
OBJS=zipfgen.o lossycon.o lossy_test.o
TARGET=lossy_test

default:all

all:$(TARGET)

$(TARGET):$(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

$(OBJS): $(DEPS)
	$(CC) -c $(DEPS)

clean:
	$(RM) $(OBJS) $(TARGET)

rebuild:clean all
