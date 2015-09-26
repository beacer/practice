#!/bin/sh - 

create_tcp_ts_chain() {
    echo "create chain ..."
    iptables -t mangle -N nf_queue_tcp_ts
    iptables -t mangle -A nf_queue_tcp_ts -m mark --mark 0 -j NFQUEUE --queue-num 8010
    iptables -t mangle -A nf_queue_tcp_ts -j MARK --set-mark 0
    iptables -t mangle -I OUTPUT -j nf_queue_tcp_ts
    iptables -t mangle -I PREROUTING -j nf_queue_tcp_ts
}

remove_tcp_ts_chain() {
    echo "remove chain ..."
    iptables -t mangle -D PREROUTING -j nf_queue_tcp_ts
    iptables -t mangle -D OUTPUT -j nf_queue_tcp_ts
    iptables -t mangle -F nf_queue_tcp_ts
    iptables -t mangle -X nf_queue_tcp_ts
}

unload_tcp_ts() {
    remove_tcp_ts_chain
    exit 1
}

trap unload_tcp_ts INT QUIT HUP

remove_tcp_ts_chain
create_tcp_ts_chain

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib/
./tcp_ts 8010 1

remove_tcp_ts_chain
