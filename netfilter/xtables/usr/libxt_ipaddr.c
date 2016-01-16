#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <xtables.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "xt_ipaddr.h"

enum {
    XT_IPADDR_OPT_SRC       = 1,
    XT_IPADDR_OPT_DST,
};

static void ipaddr_mt_init(struct xt_entry_match *match)
{
    struct xt_ipaddr_mtinfo *info = (void *)match->data;

    /* we can use the fucntion to initialze our private data,
     * here just a no-op */
    inet_pton(AF_INET6, "2001:db8::1337", &info->dst.in6);
    return;
}

static void ipaddr_mt_help(void)
{
    printf("ipaddr match options:\n"
            "[!] --ipsrc addr       Match source address of packet\n"
            "[!] --ipdst addr       Match destination address of packet\n");
    return;
}

static void ipaddr_mt6_save(const void *entry, const struct xt_entry_match *match)
{
    const struct xt_ipaddr_mtinfo *info = (const void *)match->data;

    if (info->flags & XT_IPADDR_SRC) {
        if (info->flags & XT_IPADDR_SRC_INV)
            printf("! ");
        printf("--ipsrc %s ", xtables_ip6addr_to_numeric(&info->src.in6));
    }

    if (info->flags & XT_IPADDR_DST) {
        if (info->flags & XT_IPADDR_DST_INV)
            printf("! ");
        printf("--ipdst %s ", xtables_ip6addr_to_numeric(&info->dst.in6));
    }

    return;
}

static void ipaddr_mt6_print(const void *entry, 
        const struct xt_entry_match *match, int numeric)
{
    const struct xt_ipaddr_mtinfo *info = (const void *)match->data;

    if (info->flags & XT_IPADDR_SRC) {
        printf("src IP ");
        if (info->flags & XT_IPADDR_SRC_INV)
            printf("! ");

        /* 
         * numeric for no DNS lookup, triggered by
         * 1. ip6tables -n
         * 2. ip6tables-save
         * 3. ip6tables -S
         */
        printf("%s ", numeric ? /* iptables -n */
                xtables_ip6addr_to_numeric(&info->src.in6) :
                xtables_ip6addr_to_anyname(&info->src.in6)); /* try hostname fist */
    }

    if (info->flags & XT_IPADDR_DST) {
        printf("dst IP ");
        if (info->flags & XT_IPADDR_DST_INV)
            printf("! ");
        printf("%s ", numeric ?
                xtables_ip6addr_to_numeric(&info->dst.in6) :
                xtables_ip6addr_to_anyname(&info->dst.in6));
    }

    return;
}

static int ipaddr_mt6_parse(int c, char **argv, int invert, 
    unsigned int *flags, const void *entry, 
    struct xt_entry_match **match)
{
    struct xt_ipaddr_mtinfo *info = (void *)(*match)->data;
    struct in6_addr *addrs, mask;
    unsigned int naddrs;

    switch (c) {
        case XT_IPADDR_OPT_SRC: /* --ipsrc ... */
            /* already set ? */
            if (*flags & XT_IPADDR_SRC)
                xtables_error(PARAMETER_PROBLEM, "xt_ipaddr: duplicated `--ipsrc'!");

            *flags |= XT_IPADDR_SRC;
            info->flags |= XT_IPADDR_SRC;
            if (invert) /* ! --ipsrc */
                info->flags |= XT_IPADDR_SRC_INV;

            xtables_ip6parse_any(optarg, &addrs, &mask, &naddrs);
            if (naddrs != 1)
                xtables_error(PARAMETER_PROBLEM, 
                        "%s resloves to multiple addresses!", optarg);

            memcpy(&info->src.in6, addrs, sizeof(*addrs));
            return true;

        case XT_IPADDR_OPT_DST: /* --ipdst ... */
            /* already set ? */
            if (*flags & XT_IPADDR_DST)
                xtables_error(PARAMETER_PROBLEM, "xt_ipaddr: duplicated `--ipdst'!");

            *flags |= XT_IPADDR_DST;
            info->flags |= XT_IPADDR_DST;
            if (invert) /* ! --ipsrc */
                info->flags |= XT_IPADDR_DST_INV;

            xtables_ip6parse_any(optarg, &addrs, &mask, &naddrs);
            if (naddrs != 1)
                xtables_error(PARAMETER_PROBLEM, 
                        "%s resloves to multiple addresses!", optarg);

            memcpy(&info->dst.in6, addrs, sizeof(*addrs));
            return true;

        default:
            /* not error, other xtable module may knows the option */
            return false;
    }
}

/* check after all options parsed */
static void ipaddr_mt_check(unsigned int flags)
{
    if (flags == 0)
        xtables_error(PARAMETER_PROBLEM, 
                "xt_ipaddr: specify at least `--ipsrc' or `--ipdst'");

    return;
}

static struct option ipaddr_mt_opts[] = {
    { .name = "ipsrc", .has_arg = true, .val = XT_IPADDR_OPT_SRC },
    { .name = "ipdst", .has_arg = true, .val = XT_IPADDR_OPT_DST },
    { NULL },
};

static struct xtables_match ipaddr_mt6_reg = {
    /* load suitable lib */
    .version        = XTABLES_VERSION,

    /* <name, revision, family> to identify the kernel module */
    .name           = "ipaddr",         /* consistent with kernel module */
    .revision       = 0,
    .family         = NFPROTO_IPV6,

    .size           = XT_ALIGN(sizeof(struct xt_ipaddr_mtinfo)),
    .userspacesize  = XT_ALIGN(sizeof(struct xt_ipaddr_mtinfo)),

    .help           = ipaddr_mt_help,   /* iptables -m xxx -h */
    .init           = ipaddr_mt_init,   /* initialize the rule before parse the options */
    .parse          = ipaddr_mt6_parse, /* return true if ate an cmd line option */
    .final_check    = ipaddr_mt_check,  /* check after finish opt parsing */
    .print          = ipaddr_mt6_print, /* iptables -L */
    .save           = ipaddr_mt6_save,  /* iptables-save */
    .extra_opts     = ipaddr_mt_opts,   /* 'extra' for extension specific options */
};

/* 
 * it seems a kind of magic,
 * a) macro _init is defined in xtables.h
 *   1. when module is compile with xtables (no shared libs), 
 *      it will be called by xtables.
 *   2. for sparated libs, it will expand to 
 *      'void __attribute__((constructor)) libxt_ipaddr_init(void)
 * b) __attribute__((constructor)) is a gcc extension, the function will be invoke
 *   1. before main(), 
 *   2. before *.so is loaded, refer dlopen(3)
 */
void _init(void)
{
    xtables_register_match(&ipaddr_mt6_reg);
}
