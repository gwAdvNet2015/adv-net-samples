CC=gcc

all: thread_demo server-tcp client-tcp

thread_demo: thread_demo.c
	cc -pthread thread_demo.c -o thread_demo
clean:
	@rm -rf thread_demo

server-tcp: server-tcp.c
	cc -lpthread server-tcp.c -o server-tcp

client-tcp: client-tcp.c
	cc client-tcp.c -o client-tcp
