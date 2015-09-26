#ifndef __PGEN_CONFIG_H__
#define __PGEN_CONFIG_H__
#include "list.h"

#ifndef NELEMS
#define NELEMS(a)       (sizeof(a) / sizeof((a)[0]))
#endif

typedef enum {
    PARAM_LIST,
    PARAM_PACKET,
    PARAM_PROTOCOL,
    PARAM_ETHER,
    PARAM_IPV6,
    PARAM_UDP,
    PARAM_TCP,
    PARAM_DHCPV6,
    PARAM_OPTION,
    PARAM_SRCMAC,
    PARAM_DSTMAC,
    PARAM_ETHTYPE,
    PARAM_TRAFFICCLASS,
    PARAM_FLOWLABEL,
    PARAM_NEXTHEADER,
    PARAM_HOPLIMIT,
    PARAM_SRCADDR,
    PARAM_DSTADDR,
    PARAM_SRCPORT,
    PARAM_DSTPORT,
    PARAM_CHKSUM,
    PARAM_DH6TYPE,
    PARAM_DH6XID,
    PARAM_DH6IANA,
    PARAM_DH6IAID,
    PARAM_DH6T1,
    PARAM_DH6T2,
    PARAM_DH6IAADDR,
    PARAM_DH6ADDR,
    PARAM_DH6PRFLFT,
    PARAM_DH6VLDLFT,
    PARAM_DH6STATUS,
    PARAM_DH6STATUSCODE,
    PARAM_DH6STATUSMSG,
    PARAM_DH6SERVID,
    PARAM_DH6CLNTID,
    PARAM_DH6DUIDTYPE,
    PARAM_DH6HWTYPE,
    PARAM_DH6TIME,
    PARAM_DH6LLADDR,
    PARAM_DH6PREFER,
    PARAM_FAKE, // XXX: 
} param_type_t;

typedef enum {
    PVAL_UINT,
    PVAL_STR,
    PVAL_MAC,
    PVAL_ADDR6,
    PVAL_LIST,
} param_vtype_t;

/* to build parameter tree */
struct cf_param {
    struct list_head    pa_node;
    param_type_t        pa_type;    /* yacc token is not string */
    param_vtype_t       pa_vtype;   // need it ??

    union {
        uint32_t            uint;
        char *              str;
        uint8_t             mac[6];
        struct in6_addr     addr6;
        struct list_head    params; /* low-layer cf_param{} */
    } val;

#define pa_vuint    val.uint
#define pa_vstr     val.str
#define pa_vmac     val.mac
#define pa_vaddr6   val.addr6
#define pa_vlist    val.params
};

struct cf_gene {
    int ifindex;
};

int pgen_conf_parse(char *files[], int nfile, 
        struct list_head *pkts, struct cf_gene *conf);

#endif /* __PGEN_CONFIG_H__ */
