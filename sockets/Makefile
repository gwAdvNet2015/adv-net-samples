#Use the gcc compiler
CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = 
#Objects created by makefile
OBJS = server-http web-get server-tcp client-tcp server-udp \
		client-udp

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

#Builds an http server
server-http: server-http.c
	$(CC) $(CFLAGS) -o $@ $^

#Builds a simple HTTP GET client
web-get: webget.c
	$(CC) $(CFLAGS) -o $@ $^

#Builds a tcp server
server-tcp: server-tcp.c
	$(CC) $(CFLAGS) -o $@ $^

#Builds a tcp client
client-tcp: client-tcp.c
	$(CC) $(CFLAGS) -o $@ $^

#Builds a udp server
server-udp: server-udp.c
	$(CC) $(CFLAGS) -o $@ $^

#Builds a udp client
client-udp: client-udp.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)

