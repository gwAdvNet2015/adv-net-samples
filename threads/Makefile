#Use the gcc compiler
CC = gcc
CFLAGS =
DEPS =
#Threaded execution requires the -lpthread library
LDFLAGS = -lpthread
#Objects to be compiled
OBJS = thread_demo server-tcp client-tcp

#Ensure compiling is done with all necessary dependencies
%.o: %.c $(DEPS)
	@$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

#Build a demo for using threads
thread_demo: thread_demo.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Builds a threaded tcp server
server-tcp: server-tcp.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

#Builds a tcp client
client-tcp: client-tcp.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	@rm -f $(OBJS)
