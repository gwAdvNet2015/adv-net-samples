CC = gcc
CFLAGS = -g
DEPS = 
LDFLAGS = -lpthread

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: bandwidth-test multi-bandwidth-test

bandwidth-test: bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^

multi-bandwidth-test: multi-bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@rm -f bandwidth-test multi-bandwidth-test