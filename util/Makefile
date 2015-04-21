OBJS=test.o
CFLAGS= -I. -w
#DEFINES=-DTHINK_TIME
DEPS = timer_util.h
CC = gcc
BIN = test 

all:
	make test.o
	$(CC) $(CFLAGS) $(OBJS) -o $(BIN) -lm

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm $(OBJS)
	rm $(BIN)
					
