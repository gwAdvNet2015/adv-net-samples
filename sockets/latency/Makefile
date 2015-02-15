#Use the gcc compiler
CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = 
#Objects created by makefile
OBJS = latency-count

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $^

all: $(OBJS)

rebuild: clean all

latency-count: latency-count.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
