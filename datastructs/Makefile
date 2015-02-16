#Use the gcc compiler
CC = gcc
CFLAGS = 
#List of dependency files needed for compilation
DEPS = ring_test.h
LDFLAGS = 
#Objects created by the make file
OBJS = ring_test

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

ring_test: ring_test.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
