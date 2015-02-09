#Use the gcc compiler
CC = gcc
CFLAGS = -g
DEPS = 
#Threaded execution requires the -lpthread library
LDFLAGS = -lpthread
#Objects to be compiled
OBJS = bandwidth-test multi-bandwidth-test

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $^

all: $(OBJS)

rebuild: clean all

#Creates a simple bandwidth test
bandwidth-test: bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^

#Creates a multi-threaded bandwidth test
multi-bandwidth-test: multi-bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@rm -f $(OBJS)