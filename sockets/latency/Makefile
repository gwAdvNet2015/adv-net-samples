default:
	gcc -o latency-count latency-count.c

clean:
	-rm latency-count

test test_tcp:
	./latency-count -m server -p 1234 -s tcp &
	sleep 1
	./latency-count -m client -h 127.0.0.1 -p 1234 -n 10000 -s tcp
	sleep 1
	-killall latency-count

test_udp:
	./latency-count -m server -p 1234 -s udp &
	sleep 1
	./latency-count -m client -h 127.0.0.1 -p 1234 -n 10000 -s udp
	sleep 1
	-killall latency-count
