#include <assert.h>
#include "packgen.h"
#include "config.h"

/*
 * ether ->
 *     src-mac 00:11:22:aa:bb:cc 
 *     dst-mac 00:00:00:00:a2:00
 *     eth-type 123
 */

static int prot_eth_cf2data(const struct cf_param *cf_prot, struct prot_data *prd)
{
    struct cf_param *cf_chld;
    struct ethhdr   *ethh;

    assert(cf_prot && prd && cf_prot->pa_type == PARAM_ETHER);
    prd->pd_type = PROT_ETHER;

    list_for_each_entry(cf_chld, &cf_prot->pa_vlist, pa_node) {
        ethh = &prd->pd_ether;

        switch (cf_chld->pa_type) {
        case PARAM_SRCMAC:
            memcpy(ethh->h_source, cf_chld->pa_vmac, ETH_ALEN);
            break;
        case PARAM_DSTMAC:
            memcpy(ethh->h_dest, cf_chld->pa_vmac, ETH_ALEN);
            break;
        case PARAM_ETHTYPE:
            ethh->h_proto = htons((uint16_t)cf_chld->pa_vuint);
            break;
        default:
            return EPGEN_PROTO;
        }
    }

    return EPGEN_OK;
}

static int prot_eth_load_buf(const struct prot_data *prd, uint8_t *start, size_t *size)
{
    return EPGEN_OK;
}

static int prot_eth_creat_sock(void)
{
    return EPGEN_OK;
}

static int prot_eth_transmit(int sockfd, uint8_t *buf, size_t size)
{
    return EPGEN_OK;
}

static void prot_eth_dump(const struct prot_data *prd)
{
    const struct ethhdr   *ethh;

    assert(prd && prd->pd_type == PROT_ETHER);
    ethh = &prd->pd_ether;

    fprintf(stderr, 
            "ether\n"
            "    dst %02x:%02x:%02x:%02x:%02x:%02x\n"
            "    src %02x:%02x:%02x:%02x:%02x:%02x\n"
            "    type 0x%04x\n",
            ethh->h_dest[0], ethh->h_dest[1], ethh->h_dest[2], 
            ethh->h_dest[3], ethh->h_dest[4], ethh->h_dest[5], 
            ethh->h_source[0], ethh->h_source[1], ethh->h_source[2], 
            ethh->h_source[3], ethh->h_source[4], ethh->h_source[5], 
            ntohs(ethh->h_proto));
}

static struct protocol prot_eth = {
    .pr_type        = PROT_ETHER,
    .pr_name        = "ether",
    .pr_cf2data     = prot_eth_cf2data,
    .pr_load_buf    = prot_eth_load_buf,
    .pr_creat_sock  = prot_eth_creat_sock,
    .pr_transmit    = prot_eth_transmit,
    .pr_dump        = prot_eth_dump,
};

int prot_eth_init(void)
{
    pgen_err_t err;

    if ((err = pgen_register_prot(&prot_eth)) != EPGEN_OK)
        return err;

    return EPGEN_OK;
}
