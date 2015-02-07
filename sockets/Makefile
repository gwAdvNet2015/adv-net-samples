CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = 
OBJS = server-http.c webget.c server-tcp.c client-tcp.c server-udp.c \
		client-udp.c
EXES = $(OBJS:.c=)

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXES)

$(EXES):
	$(CC) $(CFLAGS) -o $@ $@.c

# server-http: server-http.c
# 	$(CC) $(CFLAGS) -o $@ $<

# web-get: webget.c
# 	$(CC) $(CFLAGS) -o $@ $<

# server-tcp: server-tcp.c
# 	$(CC) $(CFLAGS) -o $@ $<

# client-tcp: client-tcp.c
# 	$(CC) $(CFLAGS) -o $@ $<

# server-udp: server-udp.c
# 	$(CC) $(CFLAGS) -o $@ $<

# client-udp: client-udp.c
# 	$(CC) $(CFLAGS) -o $@ $<

clean:
	@rm -f $(EXES)

