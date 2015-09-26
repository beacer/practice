#ifndef __PACKGEN_DHCP6_H__
#define __PACKGEN_DHCP6_H__
#include <stdint.h>
#include <stdbool.h>
#include "list.h"

/* 24.2 DHCP Message Types - RFC3315 */
typedef enum dh6_msg_e {
    DH6MSG_SOLICIT      = 1,
    DH6MSG_ADVERTISE    = 2,
    DH6MSG_REQUEST      = 3,
    DH6MSG_CONFIRM      = 4,
    DH6MSG_RENEW        = 5,
    DH6MSG_REBIND       = 6,
    DH6MSG_REPLY        = 7,
    DH6MSG_RELEASE      = 8,
    DH6MSG_DECLINE      = 9,
    DH6MSG_RECONFIG     = 10,
    DH6MSG_INFO_REQ     = 11,
    DH6MSG_RELAY_FORW   = 12,
    DH6MSG_RELAY_REPL   = 13,
} dh6_msg_t;

/* 24.3 DHCP Options */
typedef enum dh6_opt_e {
    /* RFC3315 */
    DH6OPT_CLIENTID     = 1,
    DH6OPT_SERVERID     = 2,
    DH6OPT_IA_NA        = 3,
    DH6OPT_IA_TA        = 4,
    DH6OPT_IAADDR       = 5,
    DH6OPT_ORO          = 6,
    DH6OPT_PREFERENCE   = 7,
    DH6OPT_ELAPSED_TIME = 8,
    DH6OPT_RELAY_MSG    = 9,
    DH6OPT_AUTH         = 11,
    DH6OPT_UNICAST      = 12,
    DH6OPT_STATUS_CODE  = 13,
    DH6OPT_RAPID_COMMIT = 14,
    DH6OPT_USER_CLASS   = 15,
    DH6OPT_VENDOR_CLASS = 16,
    DH6OPT_VENDOR_OPTS  = 17,
    DH6OPT_INTERFACE_ID = 18,
    DH6OPT_RECONF_MSG   = 19,
    DH6OPT_RECONF_ACCEPT= 20,

    /* RFC3633 */
    DH6OPT_IA_PD        = 25,
    DH6OPT_IAPREFIX     = 26,
} dh6_opt_t;

typedef enum {
    DH6DUID_LLT,
    DH6DUID_EN,
    DH6DUID_LL,
} dh6_duid_t;

/**
 * xxx_info{} are used to save essential info to construct the packet.
 */

struct dh6_iaaddr {
    uint32_t        prf_lft;
    uint32_t        vld_lft;
    struct in6_addr addr;
};

struct dh6_iapd_prefix {
    uint32_t        prf_lft;
    uint32_t        vld_lft;
    struct in6_addr addr;
    int             plen;
};

struct dh6_status_code {
    int             code;
    char *          message;
};


/* IA_NA / IA_TA / IA_PD */
struct dh6ia_info {
    uint32_t            iaid;
    uint32_t            t1;
    uint32_t            t2;

    struct list_head    opt_list; /* sub options */
};

struct dh6_duid_llt {
    uint16_t hwtype;
    uint32_t time;
    uint8_t lladdr[32];
};

struct dh6_duid_en {
    uint8_t value[0];
};

struct dh6_duid_ll {
    uint16_t hwtype;
    uint8_t lladdr[32];
};

struct dh6_duid {
    dh6_duid_t  type;
    size_t      len;
    union {
        struct dh6_duid_llt llt;
        struct dh6_duid_en en;
        struct dh6_duid_ll ll;
    } duid;
};

struct dh6opt_info {
    struct list_head    node;
    dh6_opt_t           type;

    union {
        struct dh6_duid     serv_id;
        struct dh6_duid     clnt_id;
        bool                rapidcommit;
        int                 preference;
        uint32_t            elapse_tm;
        //uint64_t            refresh_tm;
        struct dh6_iaaddr   iaaddr;
        struct dh6_iapd_prefix   iapd_prefix;
        struct dh6_status_code   status_code;

        struct dh6ia_info   iana;
        struct dh6ia_info   iapd;

        //struct list_head    dns_list;
        //struct list_head    domain_list;
        //struct list_head    prefix_list;
    } opt;
};

struct dh6_info {
    dh6_msg_t           type;
    uint32_t            xid;

    struct list_head    opt_list; /* dh6opt_info{} list */
};

/**
 * packed structs for DHCPv6 messages.
 */

struct dhcp6hdr {
    uint32_t  dh6_msgxid;
} __attribute__((__packed__));

#define DHCP6_XID_MASK  0x00ffffff

/**
 * options and sub-options 
 */

/* generic option structure */
struct dhcp6opt {
    uint16_t        type;
    uint16_t        len;
    uint8_t         value[0];
} __attribute__((__packed__));

/* IA  */
struct dhcp6o_ia {
    uint16_t        type;
    uint16_t        len;
    uint32_t        iaid;
    uint32_t        t1;
    uint32_t        t2;
    /* sub options */
} __attribute__((__packed__));

/* IA Address */
struct dhcp6o_ia_addr {
    uint16_t        type;
    uint16_t        len;
    struct in6_addr addr;
    uint32_t        prf_lft;
    uint32_t        vld_lft;
} __attribute__((__packed__));

/* IA_PD prefix */
struct dhcp6o_ia_pd_pref {
    uint16_t        type;
    uint16_t        len;
    uint32_t        prf_lft;
    uint32_t        vld_lft;
    uint8_t         plen;
    struct in6_addr prefix;
} __attribute__((__packed__));

/* Status Code */
struct dhcp6o_stcode {
    uint16_t        type;
    uint16_t        len;
    uint16_t        stcode;
} __attribute__((__packed__));

/* DUID Type 1: LLT */
struct dhcp6o_duid1 {
    uint16_t        type;
    uint16_t        hwtype;
    uint32_t        time;
    /* link-layer address */
    uint8_t         lladdr[0];
} __attribute__((__packed__));

#endif /* __PACKGEN_DHCP6_H__ */
