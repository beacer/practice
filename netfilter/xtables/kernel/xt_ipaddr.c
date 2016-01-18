#include <linux/init.h>
#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/netfilter/x_tables.h>
#include <net/ipv6.h>
#include "xt_ipaddr.h"

MODULE_AUTHOR("beacer <beacer@example.com>");
MODULE_DESCRIPTION("xtables: match source/destination address");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ip6t_ipaddr");
MODULE_ALIAS("ipt_ipaddr");

static bool ipaddr_mt6(const struct sk_buff *skb, struct xt_action_param *param)
{
    const struct xt_ipaddr_mtinfo *info = param->matchinfo; /* from userspace */
    const struct ipv6hdr *ip6h = ipv6_hdr(skb);

    /* %pI6 is newer then NIP6_FMT + NIP6 */
    pr_info("xt_ipaddr: IN=%s OUT=%s SRC=%pI6 DST=%pI6 "
            "IPSRC=%pI6 IPDST=%pI6\n",
            param->in ? param->in->name : "",
            param->out ? param->out->name : "",
            &ip6h->saddr, &ip6h->daddr,
            &info->src.in6, &info->dst.in6);

    if (info->flags & XT_IPADDR_SRC) {
        if ((ipv6_addr_cmp(&ip6h->saddr, &info->src.in6) != 0) 
                ^ !!(info->flags & XT_IPADDR_SRC_INV)) {
            pr_notice("src IP - no match\n");
            return false;
        }
    }

    if (info->flags & XT_IPADDR_DST) {
        if ((ipv6_addr_cmp(&ip6h->daddr, &info->dst.in6) != 0) 
                ^ !!(info->flags & XT_IPADDR_DST_INV)) {
            pr_notice("dst IP - no match\n");
            return false;
        }
    }

    return true;
}

static bool ipaddr_mt4(const struct sk_buff *skb, struct xt_action_param *param)
{
    const struct xt_ipaddr_mtinfo *info = param->matchinfo; /* from userspace */
    const struct iphdr *iph = ip_hdr(skb);

    pr_info("xt_ipaddr: IN=%s OUT=%s SRC=%pI4 DST=%pI4 "
            "IPSRC=%pI4 IPDST=%pI4\n",
            param->in ? param->in->name : "",
            param->out ? param->out->name : "",
            &iph->saddr, &iph->daddr,
            &info->src.in, &info->dst.in);

    if (info->flags & XT_IPADDR_SRC) {
        if ((iph->saddr != info->src.ip) ^ !!(info->flags & XT_IPADDR_SRC_INV)) {
            pr_notice("src IP - no match\n");
            return false;
        }
    }

    if (info->flags & XT_IPADDR_DST) {
        if ((iph->daddr != info->dst.ip) ^ !!(info->flags & XT_IPADDR_DST_INV)) {
            pr_notice("dst IP - no match\n");
            return false;
        }
    }

    return true;
}

static int ipaddr_mt_check(const struct xt_mtchk_param *param)
{
    const struct xt_ipaddr_mtinfo *info = param->matchinfo;

    pr_info("new rule with `-m ipaddr' in table %s hooks 0x%x\n",
            param->table, param->hook_mask);

    if (!(info->flags & (XT_IPADDR_SRC | XT_IPADDR_DST))) {
        pr_info("nothing for test\n");
        return -EINVAL;
    }

    /* sanity check*/
    switch (param->family) {
    case NFPROTO_IPV6:
        if (ntohl(info->src.ip6[0]) == 0x20010DB8) {
            pr_info("I'm sorry, can't test 2001:db8::/23\n");
            return -EPERM;
        }
        break;
    case NFPROTO_IPV4:
        /* nothing to check */
        break;
    default:
        pr_info("bad family\n");
        return -EINVAL;
    }

    return 0;
}

static void ipaddr_mt_destroy(const struct xt_mtdtor_param *param)
{
    const struct xt_ipaddr_mtinfo *info = param->matchinfo;

    switch (param->family) {
    case NFPROTO_IPV4:
        pr_info("match for addr %pI4 removed\n", &info->src.in);
        break;
    case NFPROTO_IPV6:
        pr_info("match for addr %pI6 removed\n", &info->src.in6);
        break;
    default:
        break;
    }
    return;
}

/*
 * note the tuple <name, revision, family> determines the 'xt_match',
 * so using same name for IPv4 and IPv6 is OK.
 * and 'iptables/ip6talbes' can found the correct 'xt_match' by tuple.
 */
static struct xt_match ipaddr_mt_reg[] __read_mostly = {
    {
        .name       = "ipaddr",             /* iptables ... -m ipaddr ... */
        .revision   = 0,
        .family     = NFPROTO_IPV4,
        .match      = ipaddr_mt4,
        .checkentry = ipaddr_mt_check,      /* before insert the rule */
        .destroy    = ipaddr_mt_destroy,    /* when revmove the rule */

        .matchsize  = sizeof(struct xt_ipaddr_mtinfo), /* userspace info size */
        .me         = THIS_MODULE,
    },
    {
        .name       = "ipaddr",             /* 'ip6tables ... -m ipaddr ...' */
        .revision   = 0,
        .family     = NFPROTO_IPV6,
        .match      = ipaddr_mt6,           /* match or not */
        .checkentry = ipaddr_mt_check,      /* before insert the rule */
        .destroy    = ipaddr_mt_destroy,    /* when revmove the rule */

        .matchsize  = sizeof(struct xt_ipaddr_mtinfo), /* userspace info size */
        .me         = THIS_MODULE,
    },
};

static int __init ipaddr_mt_init(void)
{
    pr_info("xt_ipaddr init\n");
    return xt_register_matches(ipaddr_mt_reg, ARRAY_SIZE(ipaddr_mt_reg));
}

static void __exit ipaddr_mt_exit(void)
{
    pr_info("xt_ipaddr exit\n");
    xt_unregister_matches(ipaddr_mt_reg, ARRAY_SIZE(ipaddr_mt_reg));
}

module_init(ipaddr_mt_init);
module_exit(ipaddr_mt_exit);
