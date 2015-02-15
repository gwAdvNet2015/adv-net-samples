#List of dependency files needed for compilation
all: linkedlist hashtable ringbuffer

hashtable:
	cd hashtable && make

linkedlist:
	cd linkedlist && make

ringbuffer:
	cd ringbuffer && make

test:
	./linkedlist/test/test-list
	./hashtable/test/test-hashtable
	./ringbuffer/test/test-ringbuffer

clean:
	cd linkedlist && make clean
	cd hashtable && make clean
	cd ringbuffer && make clean
