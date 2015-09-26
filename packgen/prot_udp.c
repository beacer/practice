#include <assert.h>
#include "packgen.h"
#include "config.h"

/*
 * udp ->
 *     src-port 547
 *     dst-port 546
 */

static int prot_udp_cf2data(const struct cf_param *cf_prot, struct prot_data *prd)
{
    struct cf_param *cf_chld;
    struct udphdr *udph;

    assert(cf_prot && prd && cf_prot->pa_type == PARAM_UDP);
    prd->pd_type = PROT_UDP;

    list_for_each_entry(cf_chld, &cf_prot->pa_vlist, pa_node) {
        udph = &prd->pd_udp;

        switch (cf_chld->pa_type) {
        case PARAM_SRCPORT:
            udph->source = cf_chld->pa_vuint;
            break;
        case PARAM_DSTPORT:
            udph->dest = cf_chld->pa_vuint;
            break;
        case PARAM_CHKSUM:
            udph->check = cf_chld->pa_vuint;
            break;
        default:
            EPGEN_PROTO;
        }
    }

    return EPGEN_OK;
}

static int prot_udp_load_buf(const struct prot_data *prd, uint8_t *start, size_t *size)
{
    return EPGEN_OK;
}

static int prot_udp_creat_sock(void)
{
    return EPGEN_OK;
}

static int prot_udp_transmit(int sockfd, uint8_t *buf, size_t size)
{
    return EPGEN_OK;
}

static void prot_udp_dump(const struct prot_data *prd)
{
    const struct udphdr   *udph;

    assert(prd && prd->pd_type == PROT_UDP);
    udph = &prd->pd_udp;

    fprintf(stderr, "udp\n    src %u dst %u chksum %u\n", 
            udph->source, udph->dest, udph->check);
}

static struct protocol prot_udp = {
    .pr_type        = PROT_UDP,
    .pr_name        = "udp",
    .pr_cf2data     = prot_udp_cf2data,
    .pr_load_buf    = prot_udp_load_buf,
    .pr_creat_sock  = prot_udp_creat_sock,
    .pr_transmit    = prot_udp_transmit,
    .pr_dump        = prot_udp_dump,
};

int prot_udp_init(void)
{
    pgen_err_t err;

    if ((err = pgen_register_prot(&prot_udp)) != EPGEN_OK)
        return err;

    return EPGEN_OK;
}
