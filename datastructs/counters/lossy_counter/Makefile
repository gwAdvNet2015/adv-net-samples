default:all

all:target

target:lossy_test

lossy_test:lossy_test.o lossycon.o zipfgen.o
	gcc -o lossy_test lossy_test.o lossycon.o zipfgen.o

zipfgen.o:zipfgen.c
	gcc -c zipfgen.c

lossycon.o:lossycon.c
	gcc -c lossycon.c

lossy_test.o:lossy_test.c
	gcc -c lossy_test.c

clean:
	rm *.o lossy_test
