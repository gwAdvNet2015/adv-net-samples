CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS =
OBJS = latency-count

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $^

all: $(OBJS)

rebuild: clean all

latency-count: latency-count.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
