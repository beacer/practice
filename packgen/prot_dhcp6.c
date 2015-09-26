#include <assert.h>
#include "packgen.h"
#include "config.h"

/* XXX: do not free @dh6i it self. */
static void dh6info_free(struct dh6_info *dh6i)
{
}

static int prot_dh6_cf2data(const struct cf_param *cf_prot, struct prot_data *prd)
{
    struct cf_param *cf_chld, *cf_subopt, *cf_subopt2;
    struct dh6_info *dh6i;
    struct dh6opt_info *dh6oi, *suboi;
    int err;

    assert(cf_prot && prd && cf_prot->pa_type == PARAM_DHCPV6);
    prd->pd_type = PROT_DHCPV6;
    dh6i = &prd->pd_dh6info;
    INIT_LIST_HEAD(&dh6i->opt_list);

    list_for_each_entry(cf_chld, &cf_prot->pa_vlist, pa_node) {
        switch (cf_chld->pa_type) {
        case PARAM_DH6TYPE:
            dh6i->type = cf_chld->pa_vuint;
            break;
        case PARAM_DH6XID:
            dh6i->xid = cf_chld->pa_vuint;
            break;
        case PARAM_DH6IANA:
            if ((dh6oi = malloc(sizeof(struct dh6opt_info))) == NULL) {
                err = EPGEN_NOMEM;
                goto errout;
            }
            dh6oi->type = DH6OPT_IA_NA;

            list_for_each_entry(cf_subopt, &cf_chld->pa_vlist, pa_node) {
                switch (cf_subopt->pa_type) {
                case PARAM_DH6IAID:
                    dh6oi->opt.iana.iaid = cf_chld->pa_vuint;
                    break;
                case PARAM_DH6T1:
                    dh6oi->opt.iana.t1 = cf_chld->pa_vuint;
                    break;
                case PARAM_DH6T2:
                    dh6oi->opt.iana.t2 = cf_chld->pa_vuint;
                    break;
                case PARAM_DH6IAADDR:
                    if ((suboi = malloc(sizeof(struct dh6opt_info))) == NULL) {
                        err = EPGEN_NOMEM;
                        free(dh6oi);
                        goto errout;
                    }
                    suboi->type = DH6OPT_IAADDR;

                    list_for_each_entry(cf_subopt2, &cf_subopt->pa_vlist, pa_node) {
                        switch (cf_subopt2->pa_type) {
                        case PARAM_DH6ADDR:
                            suboi->opt.iaaddr.addr = cf_subopt2->pa_vaddr6;
                            break;
                        case PARAM_DH6PRFLFT:
                            suboi->opt.iaaddr.prf_lft = cf_subopt2->pa_vuint;
                            break;
                        case PARAM_DH6VLDLFT:
                            suboi->opt.iaaddr.vld_lft = cf_subopt2->pa_vuint;
                            break;
                        default:
                            err = EPGEN_CONF;
                            free(suboi);
                            free(dh6oi);
                            goto errout;
                        }
                    }

                    list_add_tail(&suboi->node, &dh6oi->opt.iana.opt_list);
                    break;

                case PARAM_DH6STATUSCODE:
                    if ((suboi = malloc(sizeof(struct dh6opt_info))) == NULL) {
                        err = EPGEN_NOMEM;
                        free(dh6oi);
                        goto errout;
                    }
                    suboi->type = DH6OPT_STATUS_CODE;
                    
                    list_for_each_entry(cf_subopt2, &cf_subopt->pa_vlist, pa_node) {
                        switch (cf_subopt2->pa_type) {
                            case PARAM_DH6STATUSCODE:
                                suboi->opt.status_code.code = cf_subopt2->pa_vuint;
                                break;
                            case PARAM_DH6STATUSMSG:
                                suboi->opt.status_code.message = strdup(cf_subopt2->pa_vstr);
                                if (!suboi->opt.status_code.message) {
                                    err = EPGEN_CONF;
                                    free(suboi);
                                    free(dh6oi);
                                    goto errout;
                                }
                                break;
                            default:
                                err = EPGEN_CONF;
                                free(suboi);
                                free(dh6oi);
                                goto errout;
                        }
                    }

                    list_add_tail(&suboi->node, &dh6oi->opt.iana.opt_list);
                    break;

                default:
                    err = EPGEN_CONF;
                    goto errout;
                }
            }

            list_add_tail(&iai->node, &dh6i->opt_list);
            break;

        case PARAM_DH6SERVID:
            if ((dh6oi = malloc(sizeof(struct dh6opt_info))) == NULL) {
                err = EPGEN_NOMEM;
                goto errout;
            }
            dh6oi->type = DH6OPT_SERVERID;

            list_for_each_entry(cf_subopt, &cf_chld->pa_vlist, pa_node) {
                switch (cf_subopt->pa_type) {
                case PARAM_DH6DUIDTYPE:
                    dh6oi->opt.serv_id.type = cf_subopt->pa_vuint;
                    ...
                    break;
                case PARAM_DH6HWTYPE:
                    //dh6oi->opt.serv_id.hw = cf_subopt->pa_vuint;
                    break;
            break;
        case PARAM_DH6CLNTID:
            ... ...
            break;
        case PARAM_DH6PREFER:
            ... ...
            break;
        default:
            err = EPGEN_CONF;
            goto errout;
        }
    }

    return EPGEN_OK;

errout:
    dh6info_free(dh6i);
    return err;
}

static int prot_dh6_load_buf(const struct prot_data *prd, uint8_t *start, size_t *size)
{
    return EPGEN_OK;
}

static int prot_dh6_creat_sock(void)
{
    return EPGEN_OK;
}

static int prot_dh6_transmit(int sockfd, uint8_t *buf, size_t size)
{
    return EPGEN_OK;
}

static const char *dh6type_itoa(dh6_msg_t type)
{
    struct {
        dh6_msg_t itype;
        const char *stype;
    } dh6msg_tab[] = {
		{ DH6MSG_SOLICIT      , "Solicit" },
		{ DH6MSG_ADVERTISE    , "Advertise" },
		{ DH6MSG_REQUEST      , "Request" },
		{ DH6MSG_CONFIRM      , "Confirm" },
		{ DH6MSG_RENEW        , "Renew" },
		{ DH6MSG_REBIND       , "Rebind" },
		{ DH6MSG_REPLY        , "Reply" },
		{ DH6MSG_RELEASE      , "Release" },
		{ DH6MSG_DECLINE      , "Decline" },
		{ DH6MSG_RECONFIG     , "Reconfig" },
		{ DH6MSG_INFO_REQ     , "Info-Req" },
		{ DH6MSG_RELAY_FORW   , "Relay-Forw" },
		{ DH6MSG_RELAY_REPL   , "Relay-Repl" },
    };
    int i;

    for (i = 0; i < NELEMS(dh6msg_tab); i++) {
        if (dh6msg_tab[i].itype == type)
            return dh6msg_tab[i].stype;
    }

    return "Unknow";
}

static const char *dh6opt_itoa(dh6_opt_t opt)
{
    struct {
        dh6_opt_t iopt;
        const char *sopt;
    } dh6opt_tab[] = {
		{ DH6OPT_CLIENTID     , "Client-ID" },
		{ DH6OPT_SERVERID     , "Server-ID" },
		{ DH6OPT_IA_NA        , "IA-NA" },
		{ DH6OPT_IA_TA        , "IA-TA" },
		{ DH6OPT_IAADDR       , "IA-Addr" },
		{ DH6OPT_ORO          , "ORO" },
		{ DH6OPT_PREFERENCE   , "Preference" },
		{ DH6OPT_ELAPSED_TIME , "Elapsed-Time" },
		{ DH6OPT_RELAY_MSG    , "Relay-Msg" },
		{ DH6OPT_AUTH         , "Authentication" },
		{ DH6OPT_UNICAST      , "Unicast" },
		{ DH6OPT_STATUS_CODE  , "Status-Code" },
		{ DH6OPT_RAPID_COMMIT , "Rapid-Commit" },
		{ DH6OPT_USER_CLASS   , "User-Class" },
		{ DH6OPT_VENDOR_CLASS , "Verdor-Class" },
		{ DH6OPT_VENDOR_OPTS  , "Verdor-Options" },
		{ DH6OPT_INTERFACE_ID , "Interface-ID" },
		{ DH6OPT_RECONF_MSG   , "Reconfig-Msg" },
		{ DH6OPT_RECONF_ACCEPT, "Reconfig-Accept" },
		{ DH6OPT_IA_PD        , "IA-PD" },
		{ DH6OPT_IAPREFIX     , "IA-Prefix" },
    };
    int i;

    for (i = 0; i < NELEMS(dh6opt_tab); i++) {
        if (dh6opt_tab[i].iopt == opt)
            return dh6opt_tab[i].sopt;
    }

    return "Unknow";
}

static void pr_hex(const void *hex, size_t len)
{
    int i;
    const uint8_t *start = hex;

    for (i = 0; i < len; i++)
        fprintf(stderr, "%02X", start[i]);
}

static void dh6opt_dump(const struct dh6opt_info *dh6oi)
{
    char addr6[64];
    struct dh6ia_info *iai;
    const struct list_head *list;
    struct dh6opt_info *subopt;

    fprintf(stderr, "    Option %s ", dh6opt_itoa(dh6oi->type));
    switch (dh6oi->type) {
    case DH6OPT_SERVERID:
        fprintf(stderr, "duid %d ", dh6oi->opt.serv_id.type);
        pr_hex(&dh6oi->opt.serv_id.duid, dh6oi->opt.serv_id.len);
        break;
    case DH6OPT_CLIENTID:
        fprintf(stderr, "duid %d ", dh6oi->opt.clnt_id.type);
        pr_hex(&dh6oi->opt.clnt_id.duid, dh6oi->opt.clnt_id.len);
        break;
    case DH6OPT_RAPID_COMMIT:
        fprintf(stderr, "%d", dh6oi->opt.rapidcommit);
        break;
    case DH6OPT_PREFERENCE:
        fprintf(stderr, "%d", dh6oi->opt.preference);
        break;
    case DH6OPT_ELAPSED_TIME:
        fprintf(stderr, "%u", dh6oi->opt.elapse_tm);
        break;
    case DH6OPT_IAADDR:
        fprintf(stderr, "%s prf_lft %u vld_lft %u", 
                inet_pton(AF_INET6, &dh6oi->opt.iaaddr.addr, addr6, sizeof(addr6)) ? addr6 : "<>",
                dh6oi->opt.iaaddr.prf_lft, dh6oi->opt.iaaddr.vld_lft);
        break;
    case DH6OPT_IAPREFIX:
        fprintf(stderr, "%s/%d prf_lft %u vld_lft %u", 
                inet_pton(AF_INET6, &dh6oi->opt.iapd_prefix.addr, addr6, sizeof(addr6)) ? addr6 : "<>",
                dh6oi->opt.iapd_prefix.plen, dh6oi->opt.iapd_prefix.prf_lft, dh6oi->opt.iapd_prefix.vld_lft);
        break;
    case DH6OPT_STATUS_CODE:
        fprintf(stderr, "%d %s", dh6oi->opt.status_code.code, dh6oi->opt.status_code.message);
        break;
    case DH6OPT_IA_NA:
    case DH6OPT_IA_PD:
        if (dh6oi->type == DH6OPT_IA_NA)
            list = &dh6oi->opt.iana_list;
        else
            list = &dh6oi->opt.iapd_list;

        list_for_each_entry(iai, list, node) {
            fprintf(stderr, "iaid %u t1 %u t2 %u ", iai->iaid, iai->t1, iai->t2);
            list_for_each_entry(subopt, &iai->opt_list, node)
                dh6opt_dump(subopt);
        }
        break;
    default:
        return;
    }

    fprintf(stderr, "\n");
    return;
}

static void prot_dh6_dump(const struct prot_data *prd)
{
    const struct dh6_info *dh6i;
    const struct dh6opt_info *dh6oi;

    assert(prd && prd->pd_type == PROT_DHCPV6);
    dh6i = &prd->pd_dh6info;

    fprintf(stderr, "dhcpv6\n    type %s xid %u\n", dh6type_itoa(dh6i->type), dh6i->xid);
    list_for_each_entry(dh6oi, &dh6i->opt_list, node)
        dh6opt_dump(dh6oi);

    return;
}

static struct protocol prot_dh6 = {
    .pr_type        = PROT_DHCPV6,
    .pr_name        = "dhcpv6",
    .pr_cf2data     = prot_dh6_cf2data,
    .pr_load_buf    = prot_dh6_load_buf,
    .pr_creat_sock  = prot_dh6_creat_sock,
    .pr_transmit    = prot_dh6_transmit,
    .pr_dump        = prot_dh6_dump,
};

int prot_dh6_init(void)
{
    pgen_err_t err;

    if ((err = pgen_register_prot(&prot_dh6)) != EPGEN_OK)
        return err;

    return EPGEN_OK;
}
