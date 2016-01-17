Some Tips
=========

* if there's no xtables.h and library, need I build iptables from source code ?
  No, just install `apt-get install ipables-dev` to get `/usr/include/xtables.h` and libraries as well.

* How to use the xtables extension ?
  Basically,
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

Examples
========

0. install/remove kernel module

    pls use `dmesg` to check debug info.

    ``` bash
    $ sudo insmod xt_ipaddr.ko
    $ sudo rmmod xt_ipaddr
    ```

   > rules need be removed before removing the module, or it will be an error.

    ```bash
    $ sudo rmmod xt_ipaddr 
    rmmod: ERROR: Module xt_ipaddr is in use
    ```

1. insert/remove rules

    ``` bash
    $ sudo ip6tables -A INPUT -m ipaddr --ipsrc ::1
    $ sudo ip6tables -A INPUT -m ipaddr ! --ipsrc ::1
    $ sudo ip6tables -nvL
    Chain INPUT (policy ACCEPT 0 packets, 0 bytes)
     pkts bytes target     prot opt in     out     source               destination         
        0     0            all      *      *       ::/0                 ::/0                src IP ::1 
        0     0            all      *      *       ::/0                 ::/0                src IP ! ::1 


    # now remove them
    $ sudo ip6tables -D INPUT -m ipaddr --ipsrc ::1
    $ sudo ip6tables -D INPUT -m ipaddr ! --ipsrc ::1
    ```

2. match the rule

    ```bash
    $ ping6 -c 1 ::1
    $ sudo ip6tables -nvL
    Chain INPUT (policy ACCEPT 2 packets, 208 bytes)
     pkts bytes target     prot opt in     out     source               destination         
        2   208            all      *      *       ::/0                 ::/0                src IP ::1

    $ demsg
    [ 4004.571216] xt_ipaddr: IN=lo OUT= SRC=0000:0000:0000:0000:0000:0000:0000:0001 DST=0000:0000:0000:0000:0000:0000:0000:0001 IPSRC=0000:0000:0000:0000:0000:0000:0000:0001 IPDST=2001:0db8:0000:0000:0000:0000:0000:1337
    [ 4004.571237] xt_ipaddr: IN=lo OUT= SRC=0000:0000:0000:0000:0000:0000:0000:0001 DST=0000:0000:0000:0000:0000:0000:0000:0001 IPSRC=0000:0000:0000:0000:0000:0000:0000:0001 IPDST=2001:0db8:0000:0000:0000:0000:0000:1337
    ```
