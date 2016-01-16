#ifndef _LINUX_NETFILTER_XT_IPADDR_H
#define _LINUX_NETFILTER_XT_IPADDR_H 1
#include <linux/netfilter.h>
#if defined(KERNEL)
#include <linux/ip.h>
#include <linux/ipv6.h>
#else
#include <netinet/in.h>
#endif

enum {
    XT_IPADDR_SRC       = 0x1<<0,
    XT_IPADDR_DST       = 0x1<<1,
    XT_IPADDR_SRC_INV   = 0x1<<2,   /* need XT_IPADDR_SRC */
    XT_IPADDR_DST_INV   = 0x1<<3,   /* need XT_IPADDR_DST */
};

struct xt_ipaddr_mtinfo {
    union nf_inet_addr src, dst;    /* support ipv4/ipv6 */
    __u8 flags;
};

#endif /* _LINUX_NETFILTER_XT_IPADDR_H */
