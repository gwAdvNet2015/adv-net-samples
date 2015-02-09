CC = gcc
CFLAGS = 
DEPS = 
LDFLAGS = -lpcap
OBJS = simple_sniff

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(OBJS)

rebuild: clean all

simple_sniff: simple_sniff.c
	$(CC) -o $@ $^ $(LDFLAGS)

clean: 
	@rm -f $(OBJS)
