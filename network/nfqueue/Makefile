
CFLAGS += -Wall -g -O0

tcp_ts: tcp_ts.c
	gcc $(CFLAGS) -lnetfilter_queue $< -o $@


clean:
	rm -f tcp_ts
