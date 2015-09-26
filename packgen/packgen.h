#ifndef __PACKGEN_H__
#define __PACKGEN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip6.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <netinet/icmp6.h>
#include "list.h"
#include "ip6.h"
#include "dhcp6.h"
#include "config.h"

struct packet;
struct prot_data;

typedef enum {
    EPGEN_OK            = 0,
    EPGEN_NOMEM         = -1,
    EPGEN_PARAM         = -2,
    EPGEN_OPTION        = -3,
    EPGEN_CONF          = -4,
    EPGEN_LOADMODULE    = -5,
    EPGEN_NORESOURCE    = -6,
    EPGEN_NOTSUPPORT    = -7,
    EPGEN_PROTO         = -8,
    EPGEN_INTERNAL      = -9,
} pgen_err_t;

typedef enum {
    PROT_ETHER,
    PROT_ETHER_ARP,
    PROT_IPV4,
    PROT_IPV6,
    PROT_UDP,
    PROT_TCP,
    PROT_ICMP,
    PROT_ICMPV6,
    PROT_NDISC,
    PROT_DHCP,
    PROT_DHCPV6,
    PROT_UNKNOW         = 0xFFFFFFFF,
} prot_type_t;

struct protocol {
    prot_type_t     pr_type;
    char *          pr_name;

    /* generate protocol data and set it to packet{}. */
    int             (*pr_cf2data)(const struct cf_param *cf_prot, struct prot_data *prd);

    /* return next packet type or PACK_TYPE_NONE if finished.
     * return pgen_err_t if error happened */
    int             (*pr_load_buf)(const struct prot_data *prd, uint8_t *start, size_t *size);

    /* print this protocol part of the packet */
    void            (*pr_dump)(const struct prot_data *prd);

    /* creat socke only when needed (related packet parsed). */
    int             (*pr_creat_sock)(void);
    int             (*pr_transmit)(int sockfd, uint8_t *buf, size_t size);

    /**
     * internal use only
     */
    struct list_head    pr_node; /**/
    /* use sock{} if fd is not the only resource */
    int             pr_sockfd;
};

struct packet {
    struct list_head        pk_node;

    int                     pk_ifindex;

    /* if pk_reload is "true", 
     * then to load the packet even pk_buff is not empty. */
    bool                    pk_reload;
    uint8_t *               pk_buff;
    int                     pk_len;

    struct list_head        pk_prdata; /* list of prot_data{} */
};

/* 
 * o for those complicated protocols, e.g., DHCPv6, 
 *   it's not possible to prepare all possible combinations in advance,
 *   instead we use info struct to record all possible 
 *   informations needed to construct an message.
 *
 * o for the protocols have fixed header, to use packed struct directly.
 */
struct prot_data {
    struct list_head        pd_node;
    prot_type_t             pd_type;

    union {
        struct ethhdr       ether;
        struct ether_arp    etharp;
        //struct ip4_info     ip4info;
        struct ip6_info     ip6info;
        struct udphdr       udp;
        struct tcphdr       tcp;
        struct icmp6_hdr    icmp6;
        //struct nd_info      ndinfo;
        //struct dh4_info     dh4info;
        struct dh6_info     dh6info;
    } pd;

#define pd_ether    pd.ether
#define pd_etharp   pd.etharp
#define pd_ip4info  pd.ip4info
#define pd_ip6info  pd.ip6info
#define pd_udp      pd.udp
#define pd_tcp      pd.tcp
#define pd_icmp     pd.icmp
#define pd_icmp6    pd.icmp6
#define pd_ndinfo   pd.ndinfo
#define pd_dh4info  pd.dh4info
#define pd_dh6info  pd.dh6info
};


int pgen_register_prot(struct protocol *prot);
const char *pgen_strerror(pgen_err_t err);

/* basic protocols */
extern int prot_eth_init(void);

#endif /* __PACKGEN_H__ */
