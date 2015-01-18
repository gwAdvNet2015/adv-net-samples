default:
	gcc -o latency-count latency-count.c

clean:
	-rm latency-count

test:
	./latency-count -m server -p 1234 &
	sleep 1
	./latency-count -m client -h 127.0.0.1 -p 1234 -n 100
	-killall latency-count

