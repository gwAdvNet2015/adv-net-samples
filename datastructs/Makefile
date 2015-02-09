CC = gcc
CFLAGS = 
DEPS = ring_test.h
LDFLAGS = 
OBJS = ring_test

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

ring_test: ring_test.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
