CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = -lpthread
OBJS = thread_demo server-tcp client-tcp

%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

thread_demo: thread_demo.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

server-tcp: server-tcp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

client-tcp: client-tcp.c
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	@rm -f $(OBJS)
