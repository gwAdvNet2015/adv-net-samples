CC = gcc
CFLAGS = -g
DEPS = 
LDFLAGS = -lpthread
OBJS = bandwidth-test multi-bandwidth-test

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $^

all: $(OBJS)

rebuild: clean all

bandwidth-test: bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^

multi-bandwidth-test: multi-bandwidth-test.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	@rm -f $(OBJS)