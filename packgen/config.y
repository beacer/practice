%{
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <netinet/ip6.h>
#include <net/if.h>
#include "config.h"

#define YYDEBUG 0
#if YYDEBUG
#define TRACE       yydbg("++ %d\n", __LINE__);
int yydebug = 1;
#else
#define TRACE
#endif

extern FILE *yyin;

int yylex(void);
void yydbg(const char *fmt, ...);
void yyerror(const char *fmt, ...);

int pgen_yy_debug = 1;

/* the cf_param{} tree, note we has no 'root', 
 * we start from packet level. */
static LIST_HEAD(cf_pkts);
/* general config parameters */
static struct cf_gene cf_general;


static inline struct cf_param *param_alloc(param_type_t type)
{
    struct cf_param *param;

    if ((param = calloc(1, sizeof(struct cf_param))) == NULL) {
        yyerror("parser: %d: no memory\n", __LINE__);
        return NULL;
    }

    param->pa_type = type;
    return param;
}

static inline struct cf_param *
param_alloc_uint(param_type_t type, uint32_t value)
{
    struct cf_param *param;

    if ((param = param_alloc(type)) == NULL)
        return NULL;
    
    param->pa_vtype = PVAL_UINT;
    param->pa_vuint = value;
    return param;
}

static inline struct cf_param *
param_alloc_str(param_type_t type, const char *value)
{
    struct cf_param *param;

    if ((param = param_alloc(type)) == NULL)
        return NULL;
    
    param->pa_vtype = PVAL_STR;
    param->pa_vstr = strdup(value);
    return param;
}

static inline struct cf_param *
param_alloc_mac(param_type_t type, const uint8_t *value)
{
    struct cf_param *param;

    if ((param = param_alloc(type)) == NULL)
        return NULL;
    
    param->pa_vtype = PVAL_MAC;
    memcpy(param->pa_vmac, value, 6);
    return param;
}

static inline struct cf_param *
param_alloc_addr6(param_type_t type, const struct in6_addr *value)
{
    struct cf_param *param;

    if ((param = param_alloc(type)) == NULL)
        return NULL;
    
    param->pa_vtype = PVAL_ADDR6;
    param->pa_vaddr6 = *value;
    return param;
}

static inline struct cf_param *
param_alloc_list(param_type_t type, struct list_head *value)
{
    struct cf_param *param;

    if ((param = param_alloc(type)) == NULL)
        return NULL;
    
    param->pa_vtype = PVAL_LIST;
    if (value)
        list_replace(value, &param->pa_vlist);
    else
        INIT_LIST_HEAD(&param->pa_vlist);
    return param;
}

%}

%token BCL ECL EOS

%token OPTION PACKET PROTOCOL INTERFACE
%token ETHER IPV6 UDP TCP DHCPV6 DH6TYPE ADDRESS TIME
%token SRCMAC DSTMAC ETHTYPE SRCADDR DSTADDR SRCPORT DSTPORT CHKSUM
%token TRAFCLASS FLOWLABEL NEXTHEADER HOPLIMIT
%token IANA SERVID CLNTID PREFERENCE IAID T1 T2 IAADDR
%token PRFLFT VLDLFT CODE MESSAGE DUIDTYPE HWTYPE LLADDR XID STATUSCODE

%token V_UINT V_HEX V_STR V_ETHTYPE V_HWTYPE V_IPPROT V_PORT
%token V_ADDR6 V_MAC V_DH6TYPE V_DH6STATUSCODE V_DUIDTYPE

%union {
    int v_uint;
    char *v_str; // TODO: free
    unsigned char v_mac[6];
    struct in6_addr v_addr6;
    struct list_head *v_list;
}

%type <v_uint> V_UINT V_HEX V_DH6TYPE V_DH6STATUSCODE V_DUIDTYPE V_ETHTYPE V_HWTYPE V_IPPROT V_PORT
%type <v_str> V_STR
%type <v_addr6> V_ADDR6
%type <v_mac> V_MAC

%type <v_list> protocols protocol
%type <v_list> eth_fields eth_field
%type <v_list> ipv6_fields ipv6_field
%type <v_list> udp_fields udp_field
%type <v_list> dh6_fields dh6_field
%type <v_list> dh6_options iana_options
%type <v_list> dh6_option iana_option 
%type <v_list> dh6_iana_fields dh6_iana_field dh6_duid_fields dh6_duid_field
%type <v_list> ia_addr_fields ia_addr_field status_code_fields status_code_field

%type <v_str> PACKET ETHER IPV6 UDP DHCPV6
%type <v_str> SRCMAC DSTMAC ETHTYPE
%type <v_str> TRAFCLASS FLOWLABEL NEXTHEADER HOPLIMIT SRCADDR DSTADDR
%type <v_str> SRCPORT DSTPORT CHKSUM
%type <v_str> DH6TYPE XID

%%

statements:
        /* empty */ { TRACE; }
    |   statements statement    {TRACE;}
    ;

statement:
        option_statement { TRACE; }
    |   packet_statement { TRACE; }
    ;

option_statement:
        OPTION INTERFACE V_STR EOS
        {
            cf_general.ifindex = if_nametoindex($3);
        }
    ;

packet_statement:
        PACKET BCL protocols ECL EOS
        {
            struct cf_param *pkt;

            if ((pkt = param_alloc_list(PARAM_PACKET, $3)) == NULL)
                return -1;

            list_add_tail(&pkt->pa_node, &cf_pkts);
        }
    ;

protocols:
        /* empty */
        {
            LIST_HEAD(prot_list);
            $$ = &prot_list;
        }
    |   protocols protocol
        {
            list_add_tail($2, $1);
        }
    ;

protocol:
        PROTOCOL ETHER BCL eth_fields ECL EOS 
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_ETHER, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   PROTOCOL IPV6 BCL ipv6_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_IPV6, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   PROTOCOL UDP BCL udp_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_UDP, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   PROTOCOL DHCPV6 BCL dh6_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DHCPV6, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

eth_fields:
        /* empty */
        {
            LIST_HEAD(eth_field_list);
            $$ = &eth_field_list;
        }
    |   eth_fields eth_field
        {
            list_add_tail($2, $1);
        }
    ;

eth_field:
        SRCMAC V_MAC EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_mac(PARAM_SRCMAC, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   DSTMAC V_MAC EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_mac(PARAM_DSTMAC, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   ETHTYPE V_ETHTYPE EOS
        { 
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_ETHTYPE, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

ipv6_fields:
        /* empty */
        {
            struct cf_param *ipv6;
            if ((ipv6 = param_alloc_list(PARAM_IPV6, NULL)) == NULL)
                return -1;
            $$ = &ipv6->pa_vlist;
        }
    |   ipv6_fields ipv6_field
        {
            list_add_tail($2, $1);
        }
    ;

ipv6_field:
        TRAFCLASS V_HEX EOS 
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_TRAFFICCLASS, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   FLOWLABEL V_HEX EOS 
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_FLOWLABEL, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   NEXTHEADER V_IPPROT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_NEXTHEADER, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   HOPLIMIT V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_HOPLIMIT, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   SRCADDR V_ADDR6 EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_addr6(PARAM_SRCADDR, &$2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   DSTADDR V_ADDR6 EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_addr6(PARAM_DSTADDR, &$2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

udp_fields:
        /* empty */
        {
            LIST_HEAD(udp_field_list);
            $$ = &udp_field_list;
        }
    |   udp_fields udp_field
        {
            list_add_tail($2, $1);
        }
    ;

udp_field:
        SRCPORT V_PORT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_SRCPORT, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   DSTPORT V_PORT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DSTPORT, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   CHKSUM V_HEX EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_CHKSUM, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

dh6_fields:
        /* empty */
        {
            LIST_HEAD(dh6_field_list);
            $$ = &dh6_field_list;
        }
    |   dh6_fields dh6_field
        {
            struct cf_param *param = (struct cf_param *)$2;

            if (param->pa_vtype == PVAL_LIST) {
                list_add_tail(param->pa_vlist.next, $1);
                free(param);
            } else {
                list_add_tail($2, $1);
            }
        }
    ;

dh6_field:
        DH6TYPE V_DH6TYPE EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6TYPE, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   XID V_HEX EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6XID, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   dh6_options
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_FAKE, $1)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

dh6_options:
        /* empty */
        {
            LIST_HEAD(dh6_opt_list);
            $$ = &dh6_opt_list;
        }
    |   dh6_options dh6_option
        {
            list_add_tail($2, $1);
        }
    ;

dh6_option:
        OPTION IANA BCL dh6_iana_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DH6IANA, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   OPTION SERVID BCL dh6_duid_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DH6SERVID, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   OPTION CLNTID BCL dh6_duid_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DH6CLNTID, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   OPTION PREFERENCE V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6CLNTID, $3)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

dh6_iana_fields:
        /* empty */
        {
            LIST_HEAD(dh6_iana_field_list);
            $$ = &dh6_iana_field_list;
        }
    |   dh6_iana_fields dh6_iana_field
        {
            struct cf_param *param = (struct cf_param *)$2;

            if (param->pa_vtype == PVAL_LIST) {
                list_add_tail(param->pa_vlist.next, $1);
                free(param);
            } else {
                list_add_tail($2, $1);
            }
        }
    ;


dh6_iana_field:
        IAID V_HEX EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6IAID, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   T1 V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6T1, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
            TRACE;
        }
    |   T2 V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6T2, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
            TRACE;
        }
    |   iana_options
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_FAKE, $1)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

iana_options:
        /* empty */
        {
            LIST_HEAD(iana_opt_list);
            $$ = &iana_opt_list;
        }
    |   iana_options iana_option
        {
            list_add_tail($2, $1);
        }
    ;

iana_option:
        OPTION IAADDR BCL ia_addr_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DH6IAADDR, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   OPTION STATUSCODE BCL status_code_fields ECL EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_list(PARAM_DH6STATUS, $4)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

ia_addr_fields:
        /* empty */
        {
            LIST_HEAD(iaadd_field_list);
            $$ = &iaadd_field_list;
        }
    |   ia_addr_fields ia_addr_field
        {
            list_add_tail($2, $1);
        }
    ;

ia_addr_field:
        ADDRESS V_ADDR6 EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_addr6(PARAM_DH6ADDR, &$2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   PRFLFT V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6PRFLFT, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   VLDLFT V_UINT EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6VLDLFT, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

status_code_fields:
        /* empty */
        {
            LIST_HEAD(status_code_field_list);
            $$ = &status_code_field_list;
        }
    |   status_code_fields status_code_field
        {
            list_add_tail($2, $1);
        }
    ;

status_code_field:
        CODE V_DH6STATUSCODE EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6STATUSCODE, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   MESSAGE V_STR EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_str(PARAM_DH6STATUSMSG, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

dh6_duid_fields:
        /* empty */
        {
            LIST_HEAD(duid_field_list);
            $$ = &duid_field_list;
        }
    |   dh6_duid_fields dh6_duid_field
        {
            list_add_tail($2, $1);
        }
    ;

dh6_duid_field:
        DUIDTYPE V_DUIDTYPE EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6DUIDTYPE, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
            TRACE;
        }
    |   HWTYPE V_HWTYPE EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6HWTYPE, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   TIME V_HEX EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_uint(PARAM_DH6TIME, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    |   LLADDR V_MAC EOS
        {
            struct cf_param *param;
            if ((param = param_alloc_mac(PARAM_DH6LLADDR, $2)) == NULL)
                return -1;
            $$ = &param->pa_node;
        }
    ;

%%

void yydbg(const char *fmt, ...)
{
    va_list ap;

    if (!pgen_yy_debug)
        return;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
}

void yyerror(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    exit(1);
}

int pgen_conf_parse(char *files[], int nfile, 
        struct list_head *pkts, struct cf_gene *conf)
{
    int i;

    if (!files || nfile == 0 || !pkts || !conf)
        return -1;

    for (i = 0; i < nfile; i++) {
        if (!files[i])
            goto errout;

        if ((yyin = fopen(files[i], "rb")) == NULL) {
            fprintf(stderr, "%s: can not open file %s\n", __FUNCTION__, files[i]);
            goto errout;
        }

        if (yyparse() != 0) {
            fprintf(stderr, "%s: fail to parse file %s\n", __FUNCTION__, files[i]);
            fclose(yyin);
            goto errout;
        }

        fclose(yyin);
    }

    list_replace(&cf_pkts, pkts);
    *conf = cf_general;
    return 0;

errout:
    // TODO: clean up 
    return -1;
}
