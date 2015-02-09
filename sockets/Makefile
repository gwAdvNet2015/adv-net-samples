CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = 
OBJS = server-http web-get server-tcp client-tcp server-udp \
		client-udp

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

server-http: server-http.c
	$(CC) $(CFLAGS) -o $@ $^

web-get: webget.c
	$(CC) $(CFLAGS) -o $@ $^

server-tcp: server-tcp.c
	$(CC) $(CFLAGS) -o $@ $^

client-tcp: client-tcp.c
	$(CC) $(CFLAGS) -o $@ $^

server-udp: server-udp.c
	$(CC) $(CFLAGS) -o $@ $^

client-udp: client-udp.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)

