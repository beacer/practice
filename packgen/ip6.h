#ifndef __PACKGEN_IP6_H__
#define __PACKGEN_IP6_H__
#include <stdint.h>
#include "list.h"

struct ip6opt_info {
    struct list_head    node;

    uint8_t             type;
    uint8_t             len;
    uint8_t             value[0];
    /* followed bye option value */
};

struct ip6ext_info {
    struct list_head    node;
    int                 prot; /* ID of current extension header */

    union {
        struct ip6_hbh      hbhopt;
        struct ip6_dest     destopt;
        struct ip6_rthdr0   rthdr0; /* type 0 routing header */
        struct ip6_frag     frag;
    } exthdr;

    /* hop-by-hop options, dest-options headers */
    struct list_head    opt_list;

    /* routing headers */
    struct in6_addr     *addrs;
    int                 naddr;
};

struct ip6_info {
    /* basic header */
    uint8_t             tc;
    uint32_t            flow_id;
    uint16_t            plen;
    uint8_t             nexthdr;
    uint8_t             hoplimit;
    struct in6_addr     srcaddr;
    struct in6_addr     dstaddr;

    /* extension headers */
    struct list_head    ext_list;
};

#endif /* __PACKGEN_IP6_H__ */
