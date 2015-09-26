#include <stdio.h>
#include <assert.h>
#include "packgen.h"
#include "config.h"

#if 0
struct parser {
};

struct scheduler {
};
#endif


//static const char *config_file = "/etc/packgen.conf"; /* default */

/**
 * loaders for all protocol{}
 * to load ready to send buffer to send from packet{} structure.
 */
static LIST_HEAD(protocols);

/**
 * all packet{} info parsed from config file.
 * the real packet (frame) can be load form these packet{}.
 */
static LIST_HEAD(packets);

/**
 * information parsed from command line arguments or config files.
 */
static LIST_HEAD(cf_pkts);
static struct cf_gene pgen_conf;
static bool test_conf = false;
static bool verbose = false;

static void usage(void)
{
    fprintf(stderr, 
            "USAGE\n"
            "    packgen [OPTION]... FILE ...\n"
            "\n"
            "OPTIONS\n"
            "    -v         print verbose messages.\n"
            "    -h         print this help info.\n"
            "    -t         test the config files and quit.\n"
            "\n"
            "EXAMPLES\n"
            "\n");
}

static int parse_args(const char *prog, int argc, char *argv[])
{
    int opt;

    if (argc <= 1) {
        usage();
        exit(1);
    }

    while ((opt = getopt(argc, argv, "vht")) != -1) {
        switch (opt) {
        case 'v':
            verbose = true;
            break;
        case 'h':
            usage();
            exit(0);
            break;
        case 't':
            test_conf = true;
            break;
        default: /* '?' */
            return EPGEN_OPTION;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "%s: at least one config file needed.\n", prog);
        return EPGEN_OPTION;
    }

    return EPGEN_OK;
}

static pgen_err_t prot_init_basic()
{
    int err;

    if ((err = prot_eth_init()) != EPGEN_OK)
        return err;
    if ((err = prot_ipv6_init()) != EPGEN_OK)
        return err;
    if ((err = prot_udp_init()) != EPGEN_OK)
        return err;
    if ((err = prot_dh6_init()) != EPGEN_OK)
        return err;

    return EPGEN_OK;
}

/*
 * How about load module when parsing config file (prot_mod += ospf) ?
 * Emm, it means user have to define which module to load expect the packet.
 * Module is introduced to extend new protocol, and user should focus on 
 * packet definition as well the schedule.
 */
static int prot_init_mods(void)
{
    // TODO:
}

static struct protocol *prot_find(prot_type_t type)
{
    struct protocol *prot;

    list_for_each_entry(prot, &protocols, pr_node) {
        if (prot->pr_type == type)
            return prot;
    }

    return NULL;
}

static prot_type_t param_typetoprot(param_type_t type)
{
    int i;
    struct {
        prot_type_t  param_type;
        prot_type_t  prot_type;
    } prot_tab[] = {
        {PARAM_ETHER,   PROT_ETHER},
        {PARAM_IPV6,    PROT_IPV6},
        {PARAM_UDP,     PROT_UDP},
        {PARAM_TCP,     PROT_TCP},
        {PARAM_DHCPV6,  PROT_DHCPV6},
    };

    for (i = 0; i < NELEMS(prot_tab); i++) {
        if (prot_tab[i].param_type == type)
            return prot_tab[i].prot_type;
    }

    return PROT_UNKNOW;
}

static const char *param_typetostr(param_type_t type)
{
    int i;
    struct {
        param_type_t itype;
        const char *stype;
    } param_tab[] = {
        { PARAM_LIST, "list"},
        { PARAM_PACKET, "packet"},
        { PARAM_PROTOCOL, "protocol"},
        { PARAM_ETHER, "ether"},
        { PARAM_IPV6, "ipv6"},
        { PARAM_UDP, "udp"},
        { PARAM_TCP, "tcp"},
        { PARAM_DHCPV6, "dhcpv6"},
        { PARAM_OPTION, "option"},
        { PARAM_SRCMAC, "src-mac"},
        { PARAM_DSTMAC, "dst-mac"},
        { PARAM_ETHTYPE, "eth-type"},
        { PARAM_TRAFFICCLASS, "traffic-class"},
        { PARAM_FLOWLABEL, "flow-label"},
        { PARAM_NEXTHEADER, "next-header"},
        { PARAM_HOPLIMIT, "hop-limit"},
        { PARAM_SRCADDR, "src-addr"},
        { PARAM_DSTADDR, "dst-addr"},
        { PARAM_SRCPORT, "src-port"},
        { PARAM_DSTPORT, "dst-port"},
        { PARAM_CHKSUM, "checksum"},
        { PARAM_DH6TYPE, "type"},
        { PARAM_DH6XID, "xid"},
        { PARAM_DH6IANA, "iana"},
        { PARAM_DH6IAID, "iapd"},
        { PARAM_DH6T1, "t1"},
        { PARAM_DH6T2, "t2"},
        { PARAM_DH6IAADDR, "ia-addr"},
        { PARAM_DH6ADDR, "address"},
        { PARAM_DH6PRFLFT, "prf-lft"},
        { PARAM_DH6VLDLFT, "vld-lft"},
        { PARAM_DH6STATUS, "status-code"},
        { PARAM_DH6STATUSCODE, "code"},
        { PARAM_DH6STATUSMSG, "message"},
        { PARAM_DH6SERVID, "server-id"},
        { PARAM_DH6CLNTID, "client-id"},
        { PARAM_DH6DUIDTYPE, "duid-type" },
        { PARAM_DH6HWTYPE, "hw-type" },
        { PARAM_DH6TIME, "time" },
        { PARAM_DH6LLADDR, "ll-addr" },
        { PARAM_DH6PREFER, "preference"},
    };

    for (i = 0; i < NELEMS(param_tab); i++) {
        if (type == param_tab[i].itype)
            return param_tab[i].stype;
    }

    return "unknow";
}

static void print_tabs(int ntab)
{
    while (ntab--)
        fprintf(stderr, "    ");
}

static void print_param(const struct cf_param *param)
{
    static int tab = 1;
    char addr6[64];
    const struct cf_param *sub_param;

    print_tabs(tab);

    fprintf(stderr, "%s ", param_typetostr(param->pa_type));
    switch (param->pa_vtype) {
    case PVAL_UINT:
        fprintf(stderr, "%u\n", param->pa_vuint);
        break;
    case PVAL_STR:
        fprintf(stderr, "%s\n", param->pa_vstr);
        break;
    case PVAL_MAC:
        fprintf(stderr, "%02x:%02x:%02x:%02x:%02x:%02x\n", 
                param->pa_vmac[0], param->pa_vmac[1], param->pa_vmac[2], 
                param->pa_vmac[3], param->pa_vmac[4], param->pa_vmac[5]);
        break;
    case PVAL_ADDR6:
        fprintf(stderr, "%s\n", 
                inet_ntop(AF_INET6, &param->pa_vaddr6, addr6, sizeof(addr6) ? addr6 : "?"));
        break;
    case PVAL_LIST:
        fprintf(stderr, "->\n");
        tab++;
        list_for_each_entry(sub_param, &param->pa_vlist, pa_node)
            print_param(sub_param);
        tab--;
        break;
    default:
        fprintf(stderr, "(%d)\n", param->pa_type);
        break;
    }

    return;
}

static void config_dump(const struct cf_gene *cfg, const struct list_head *pkts)
{
    struct cf_param *param;

    fprintf(stderr, "General Configurations\n");
    fprintf(stderr, "    ifindex %d\n", cfg->ifindex);

    fprintf(stderr, "Packets\n");
    list_for_each_entry(param, pkts, pa_node)
        print_param(param);

    return;
}

static void packets_free(struct list_head *pkts)
{
    // TODO:

    INIT_LIST_HEAD(pkts);
    return;
}

static int packets_init(const struct list_head *cf_pkts, struct list_head *pkts)
{
    const struct cf_param *cf_pkt, *cf_prot;
    struct packet *pkt;
    int err = EPGEN_INTERNAL;
    prot_type_t pt;
    struct protocol *prot;
    struct prot_data *prd;

    /* for each packet */
    list_for_each_entry(cf_pkt, cf_pkts, pa_node) {
        if (cf_pkt->pa_type != PARAM_PACKET) {
            fprintf(stderr, "%s: param is not packet\n", __FUNCTION__);
            continue;
        }

        if ((pkt = calloc(1, sizeof(struct packet))) == NULL) {
            err = EPGEN_NOMEM;
            goto errout;
        }
        INIT_LIST_HEAD(&pkt->pk_prdata);

        /* for each protocol - load protocol specific info */
        list_for_each_entry(cf_prot, &cf_pkt->pa_vlist, pa_node) {
            pt = param_typetoprot(cf_prot->pa_type);
            if ((prot = prot_find(pt)) == NULL) {
                // XXX: handle non-protocol param here.
                continue;
            }
            
            /* load protcol data (header) */
            if ((prd = calloc(1, sizeof(struct prot_data))) == NULL) {
                err = EPGEN_NOMEM;
                goto errout;
            }
            if ((err = prot->pr_cf2data(cf_prot, prd)) != EPGEN_OK) {
                free(prd);
                goto errout;
            }
            list_add_tail(&prd->pd_node, &pkt->pk_prdata);

            /* the socket type is decided by it's lowest protocol type */
            if (cf_prot == list_first_entry(&cf_pkt->pa_vlist, struct cf_param, pa_node)) {
                if ((err = prot->pr_creat_sock()) != EPGEN_OK) {
                    fprintf(stderr, "%s: fail to create socket\n", __FUNCTION__);
                    goto errout;
                }
            }
        }

        list_add_tail(&pkt->pk_node, pkts);
    }

    return EPGEN_OK;

errout:
    packets_free(pkts);
    return err;
}

static void packets_dump(const struct list_head *pkts)
{
    struct packet *pkt;
    struct protocol *prot;
    struct prot_data *prd;
    int i;

    assert(pkts);

    list_for_each_entry(pkt, pkts, pk_node) {
        fprintf(stderr, "[ Packet ]\n");
        fprintf(stderr, "    ifindex: %d\n", pkt->pk_ifindex);

        list_for_each_entry(prd, &pkt->pk_prdata, pd_node) {
            prot = prot_find(prd->pd_type);
            if (prot->pr_dump)
                prot->pr_dump(prd);
        }

        if (pkt->pk_buff) {
            fprintf(stderr, "    buffer:");
            for (i = 0; i < pkt->pk_len; i++) {
                if (i % 16 == 0)
                    fprintf(stderr, "\n    ");
                fprintf(stderr, "%02X ", pkt->pk_buff[i]);
            }
            fprintf(stderr, "\n");
        }
    }
}

const char *pgen_strerror(pgen_err_t err)
{
    return "xxx";
}

int pgen_register_prot(struct protocol *prot)
{
    if (!prot || !prot->pr_name || !prot->pr_cf2data || !prot->pr_load_buf
            || !prot->pr_creat_sock || !prot->pr_transmit)
        return EPGEN_INTERNAL;

    list_add_tail(&prot->pr_node, &protocols);
    return EPGEN_OK;
}

int main(int argc, char *argv[])
{
    int err;
    char *prog;

    if ((prog = strrchr(argv[0], '/')) != NULL)
        prog++;
    else
        prog = argv[0];

    if (parse_args(prog, argc, argv) != EPGEN_OK)
        exit(1);
    argc -= optind;
    argv += optind;

    if ((err = pgen_conf_parse(argv, argc, &cf_pkts, &pgen_conf)) != 0) {
        fprintf(stderr, "%s: fail to parse packet(.pgen) file.\n", prog);
        exit(1);
    }

    if (test_conf) {
        config_dump(&pgen_conf, &cf_pkts);
        exit(0);
    }

    if ((err = prot_init_basic()) != EPGEN_OK) {
        fprintf(stderr, "%s: fail to init build-in protocols: %s\n", prog, pgen_strerror(err));
        exit(1);
    }

    if ((err = prot_init_mods()) != EPGEN_OK) {
        fprintf(stderr, "%s: fail to init protocols modules: %s\n", prog, pgen_strerror(err));
        exit(1);
    }

    if (packets_init(&cf_pkts, &packets) != EPGEN_OK) {
        fprintf(stderr, "%s: fail to init packets: %s\n", prog, pgen_strerror(err));
        exit(1);
    }

    if (verbose)
        packets_dump(&packets);

    /* sending schedule */

    exit(0);
}
