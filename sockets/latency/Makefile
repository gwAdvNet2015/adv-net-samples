CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = -lpthread

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: latency-count

latency-count: latency-count.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm latency-count
