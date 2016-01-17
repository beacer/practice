
Q: if there's no xtables.h and library, need I build iptables from source code ?
A: No, just install `apt-get install ipables-dev` to get `/usr/include/xtables.h` and libraries as well.

Q: How to use the xtables extension
A: Basically,
  1. install kernel module
  2. install userspace library
  3. run iptalbes/ip6tables command 
  > Note: to use `ip6tables` instead of `iptables` if the xt_match.family is `NFPROTO_IPV6`.

    ```bash
    $ cd kernel
    $ insmod xt_ipaddr.ko
    $ cd ../usr
    $ sudo cp libxt_ipaddr.so /lib/xtables/
    $ sudo ip6tables -m ipaddr -h
    ```
