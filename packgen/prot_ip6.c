#include <assert.h>
#include "packgen.h"
#include "config.h"

/*
 * ipv6 ->
 *     traffic-class 0
 *     flow-label 0
 *     next-header 17
 *     hop-limit 64
 *     src-addr fe80::01:02:03ff:fe04:0506
 *     dst-addr fe80::200:ff:fe00:01:01
 */

static int prot_ipv6_cf2data(const struct cf_param *cf_prot, struct prot_data *prd)
{
    struct cf_param *cf_chld;
    struct ip6_info *ip6i;

    assert(cf_prot && prd && cf_prot->pa_type == PARAM_IPV6);
    prd->pd_type = PROT_IPV6;

    list_for_each_entry(cf_chld, &cf_prot->pa_vlist, pa_node) {
        ip6i = &prd->pd_ip6info;

        switch (cf_chld->pa_type) {
        case PARAM_TRAFFICCLASS:
            ip6i->tc = cf_chld->pa_vuint;
            break;
        case PARAM_FLOWLABEL:
            ip6i->flow_id = cf_chld->pa_vuint;
            break;
        case PARAM_NEXTHEADER:
            ip6i->nexthdr = cf_chld->pa_vuint;
            break;
        case PARAM_HOPLIMIT:
            ip6i->hoplimit = cf_chld->pa_vuint;
            break;
        case PARAM_SRCADDR:
            ip6i->srcaddr = cf_chld->pa_vaddr6;
            break;
        case PARAM_DSTADDR:
            ip6i->dstaddr = cf_chld->pa_vaddr6;
            break;
        default:
            return EPGEN_PROTO;
        }
    }

    return EPGEN_OK;
}


static void prot_ipv6_dump(const struct prot_data *prd)
{
    const struct ip6_info *ip6i;
    char saddr[64], daddr[64];

    assert(prd && prd->pd_type == PROT_IPV6);
    ip6i = &prd->pd_ip6info;

    fprintf(stderr, 
            "ipv6\n"
            "    tc %u flow %u next %u hop %u\n"
            "    src %s\n"
            "    dst %s\n",
            ip6i->tc, ip6i->flow_id, ip6i->nexthdr, ip6i->hoplimit,
            inet_ntop(AF_INET6, &ip6i->srcaddr, saddr, sizeof(saddr)) ? saddr : "<none>",
            inet_ntop(AF_INET6, &ip6i->dstaddr, daddr, sizeof(daddr)) ? daddr : "<none>");
}

static int prot_ipv6_load_buf(const struct prot_data *prd, uint8_t *start, size_t *size)
{
    return EPGEN_OK;
}

static int prot_ipv6_creat_sock(void)
{
    return EPGEN_OK;
}

static int prot_ipv6_transmit(int sockfd, uint8_t *buf, size_t size)
{
    return EPGEN_OK;
}

static struct protocol prot_ipv6 = {
    .pr_type        = PROT_IPV6,
    .pr_name        = "ipv6",
    .pr_cf2data     = prot_ipv6_cf2data,
    .pr_load_buf    = prot_ipv6_load_buf,
    .pr_creat_sock  = prot_ipv6_creat_sock,
    .pr_transmit    = prot_ipv6_transmit,
    .pr_dump        = prot_ipv6_dump,
};

int prot_ipv6_init(void)
{
    pgen_err_t err;

    if ((err = pgen_register_prot(&prot_ipv6)) != EPGEN_OK)
        return err;

    return EPGEN_OK;
}
