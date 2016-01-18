#include <linux/init.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/netfilter/x_tables.h>
#include <net/ipv6.h>
#include "xt_ECHO.h"

MODULE_AUTHOR("beacer <beacer@example.com>");
MODULE_DESCRIPTION("xtables: RFC862 \"echo\" protocol");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ip6t_ECHO");
MODULE_ALIAS("ipt_ECHO");

static unsigned int echo_tg6(struct sk_buff *skb, 
        const struct xt_action_param *param)
{
    const struct udphdr *udph;
    const struct ipv6hdr *ip6h;
    struct udphdr *newudph, udpbuf;
    struct ipv6hdr *newip6h;
    struct sk_buff *newskb;
    unsigned int data_len;
    void *payload;

    if (skb_linearize(skb) < 0)
        return NF_DROP;

    /* L3 header is linear when handled by ip_tables/ip6_tables */
    ip6h = ipv6_hdr(skb);

    /**
     * XXX: 
     * 1. why use skb_header_pointer for linearize ? skb_linearize is not enough ?
     * 2. if skb_header_pointer is needed for linearize, why pass NULL as 4th param
     *    when handling payload later ?
     *    note payload is not linearized by skb_header_pointer() here.
     */

    /* 1. param.thoff is transport-header offset set by iptables/ip6tables 
     *    after L3 match (IPv4/IPv6), will not be set by L2 (ebtables).
     * 2. 2nd and 3rd parameter of skb_header_pointer is the start and size 
     *    of data to get
     * 3. return NULL means no enough data to get.
     * 4. to make sure L4 header is linear, skb_header_pointer is used,
     *    it can handle non-linear skb, it will copy data to it's 4th buffer
     *    and return the address or just return the origin offset without copy. */
    udph = skb_header_pointer(skb, param->thoff, sizeof(struct udphdr), &udpbuf);
    if (!udph || ntohs(udph->len) <= sizeof(struct udphdr))
        return NF_DROP;

    /* 
     * use GFP_ATOMIC since target function may invoked in interrupt context 
     * for length calculating:
     * 1. max link-layer header
     * 2. IPv6 header without extension (we donot echo extension header)
     * 3. UDP header as well as payload (udph->len including both)
     */
    newskb = alloc_skb(LL_MAX_HEADER + sizeof(struct ipv6hdr) + 
            ntohs(udph->len), GFP_ATOMIC);
    if (!newskb)
        return NF_DROP;

    skb_reserve(newskb, LL_MAX_HEADER);

    /* handle L3 header */
    skb_reset_network_header(newskb);
    newip6h = (void *)skb_put(newskb, sizeof(struct ipv6hdr));
    newip6h->version    = ip6h->version;
    newip6h->priority   = ip6h->priority;
    memcpy(newip6h->flow_lbl, ip6h->flow_lbl, sizeof(newip6h->flow_lbl));
    newip6h->nexthdr    = IPPROTO_UDP; /* skill extension headers */
    newip6h->saddr      = ip6h->daddr; /* reverse */
    newip6h->daddr      = ip6h->saddr;

    /* handle L4 header */
    skb_reset_transport_header(newskb);
    newudph = (void *)skb_put(newskb, sizeof(struct udphdr));
    newudph->source     = udph->dest;   /* reverse */
    newudph->dest       = udph->source;
    newudph->len        = udph->len;

    /* payload */
    data_len = htons(udph->len) - sizeof(struct udphdr);
    payload = skb_header_pointer(skb, param->thoff + sizeof(struct udphdr), data_len, NULL);
    memcpy(skb_put(newskb, data_len), payload, data_len);

    /* checksum */
    newudph->check = 0;
    newudph->check = csum_ipv6_magic(&newip6h->saddr, &newip6h->daddr,
            ntohs(newudph->len), IPPROTO_UDP,
            csum_partial(newudph, ntohs(newudph->len), 0));

    /* new skb get the same conntrack entry as old one  */
    nf_ct_attach(newskb, skb);
    /* select the output route and send out the skb */
    //XXX: get hang somehow !!
    //ip6_local_out(newskb);
    return NF_DROP; /* alreay handled, do not pass it to UDP layer. */
}

static struct xt_target echo_tg_reg __read_mostly = {
    .name       = "ECHO",
    .revision   = 0,
    .family     = NFPROTO_IPV6,
    .proto      = IPPROTO_UDP,
    .hooks      = (1 << NF_INET_LOCAL_IN)
                | (1 << NF_INET_FORWARD)
                | (1 << NF_INET_LOCAL_OUT),
    .target     = echo_tg6,
    .me         = THIS_MODULE,

    /**
     * .targetsize is zero, means neither option needed (from userspace) 
     * nor private data in kernel.
     * .checkentry & .destroy is NULL means nothing for insert/remove rule.
     */
};

static int __init echo_tg_init(void)
{
    pr_info("xt_ECHO: init\n");
    return xt_register_target(&echo_tg_reg);
}

static void __exit echo_tg_exit(void)
{
    pr_info("xt_ECHO: exit\n");
    xt_unregister_target(&echo_tg_reg);
}

module_init(echo_tg_init);
module_exit(echo_tg_exit);
