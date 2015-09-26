%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include "y.tab.h"


#if 0
#define TRACE do { \
        if (pgen_yy_debug) \
            yydbg("== %d yytext %s\n", __LINE__, yytext); \
    } while (0)
#else
#define TRACE
#endif

extern void yydbg(const char *fmt, ...);
extern void yyerror(const char *fmt, ...);
extern int yyparse(void);

%}

    /* abbreviation */
hexdigit    [0-9a-fA-F]
hexpair     {hexdigit}{hexdigit}

    /* start-states, for conflict keywords */
%s  S_ETHTYPE
%s  S_IPPROT
%s  S_PORT
%s  S_DHCPV6
%s  S_STATUS
%s  S_DUID

%%

    /* seperators */
\{          { TRACE; return BCL; }
\}          { TRACE; return ECL; }
;           { TRACE; return EOS; }

    /* key words */
    /* start-states first, then default items */

<S_ETHTYPE>arp|ipv4|ipv6|[0-9]+  { 
    TRACE;
    if (strcmp(yytext, "arp") == 0) {
        yylval.v_uint = ETHERTYPE_ARP;
    } else if (strcmp(yytext, "ipv4") == 0) {
        yylval.v_uint = ETHERTYPE_IP;
    } else if (strcmp(yytext, "ipv6") == 0) {
        yylval.v_uint = ETHERTYPE_IPV6;
    } else {
        yyerror("invalid protocol %s\n", yytext);
    }

    BEGIN INITIAL;
    return V_ETHTYPE;
}

<S_IPPROT>(udp|tcp|icmp|icmpv6|dhcpv6)  { 
    TRACE;
    if (strcmp(yytext, "udp") == 0) {
        yylval.v_uint = IPPROTO_UDP;
    } else if (strcmp(yytext, "tcp") == 0) {
        yylval.v_uint = IPPROTO_TCP;
    } else if (strcmp(yytext, "icmp") == 0) {
        yylval.v_uint = IPPROTO_ICMP;
    } else if (strcmp(yytext, "icmpv6") == 0) {
        yylval.v_uint = IPPROTO_ICMPV6;
    } else if (isdigit(yytext[0])) {
        yylval.v_uint = atoi(yytext);
    } else {
        yyerror("invalid protocol type %s\n", yytext);
    }

    BEGIN INITIAL;
    
    return V_IPPROT;
}

<S_PORT>(ftp|telnet|smtp|dhcps|dhcpc|bootps|bootpc|dhcp6s|dhcp6c|[0-9]+) {
    TRACE;
    if (strcmp(yytext, "ftp") == 0) {
        yylval.v_uint = IPPORT_FTP;
    } else if (strcmp(yytext, "telnet") == 0) {
        yylval.v_uint = IPPORT_TELNET;
    } else if (strcmp(yytext, "smtp") == 0) {
        yylval.v_uint = IPPORT_SMTP;
    } else if (strcmp(yytext, "dhcps") == 0 || strcmp(yytext, "bootps") == 0) {
        yylval.v_uint = 67;
    } else if (strcmp(yytext, "dhcpc") == 0 || strcmp(yytext, "bootpc") == 0) {
        yylval.v_uint = 68;
    } else if (strcmp(yytext, "dhcp6c") == 0) {
        yylval.v_uint = 546;
    } else if (strcmp(yytext, "dhcp6s") == 0) {
        yylval.v_uint = 547;
    } else if (isdigit(yytext[0])) {
        yylval.v_uint = atoi(yytext);
    } else {
        yyerror("invalid port %s\n", yytext);
    }

    BEGIN INITIAL;
    return V_PORT;
}

<S_DHCPV6>type  { TRACE; BEGIN INITIAL; return DH6TYPE; }

<S_STATUS>(Success|UnspecFail|NoAddrsAvail|NoBinding|NotOnLink|UseMulticast|NoPrefixAvail|UnknowQueryType|MalformedQuer|NotConfigured|NotAllowed|QueryTerminated|[0-9]+) {
    TRACE;
    /* RFC3315/3633/5007 */
    if (strcmp(yytext, "Success") == 0) {
        yylval.v_uint = 0;
    } else if (strcmp(yytext, "UnspecFail") == 0) {
        yylval.v_uint = 1;
    } else if (strcmp(yytext, "NoAddrsAvail") == 0) {
        yylval.v_uint = 2;
    } else if (strcmp(yytext, "NoBinding") == 0) {
        yylval.v_uint = 3;
    } else if (strcmp(yytext, "NotOnLink") == 0) {
        yylval.v_uint = 4;
    } else if (strcmp(yytext, "UseMulticast") == 0) {
        yylval.v_uint = 5;
    } else if (strcmp(yytext, "NoPrefixAvail") == 0) {
        yylval.v_uint = 6;
    } else if (strcmp(yytext, "UnknowQueryType") == 0) {
        yylval.v_uint = 7;
    } else if (strcmp(yytext, "MalformedQuery") == 0) {
        yylval.v_uint = 8;
    } else if (strcmp(yytext, "NotConfigured") == 0) {
        yylval.v_uint = 9;
    } else if (strcmp(yytext, "NotAllowed") == 0) {
        yylval.v_uint = 10;
    } else if (strcmp(yytext, "QueryTerminated") == 0) {
        yylval.v_uint = 11;
    } else if (isdigit(yytext[0])) {
        yylval.v_uint = atoi(yytext);
    } else {
        yyerror("invalid DHCP(v4/v6) Status Code %s\n", yytext);
    }

    BEGIN INITIAL;
    return V_DH6STATUSCODE;
}

<S_DUID>time        { TRACE; BEGIN INITIAL; return TIME; }

option      { TRACE; return OPTION; }
packet      { TRACE; return PACKET; }
protocol    { TRACE; return PROTOCOL; }
interface   { TRACE; return INTERFACE; }

ether       { TRACE; return ETHER; }
ipv6        { TRACE; return IPV6; }
udp         { TRACE; return UDP; }
tcp         { TRACE; return TCP; }
dhcpv6      { TRACE; BEGIN S_DHCPV6; return DHCPV6; }

src-mac     { TRACE; return SRCMAC; }
dst-mac     { TRACE; BEGIN S_ETHTYPE; return DSTMAC; }
eth-type    { TRACE; return ETHTYPE; }

traffic-class   { TRACE; return TRAFCLASS; }
flow-label      { TRACE; return FLOWLABEL; }
next-header     { TRACE; BEGIN S_IPPROT; return NEXTHEADER; }
hop-limit       { TRACE; return HOPLIMIT; }
src-addr        { TRACE; return SRCADDR; }
dst-addr        { TRACE; return DSTADDR; }

src-port        { TRACE; BEGIN S_PORT; return SRCPORT; }
dst-port        { TRACE; BEGIN S_PORT; return DSTPORT; }

xid             { TRACE; return XID; }
ia-na           { TRACE; return IANA; }
server-id       { TRACE; BEGIN S_DUID; return SERVID; }
client-id       { TRACE; BEGIN S_DUID; return CLNTID; }
preference      { TRACE; return PREFERENCE; }

(Solicit|Advertise|Request|Confirm|Renew|Rebind|Decline|Release|Reply|Reconfigure|Info-Request|Relay-Forward|Relay-Reply) {
    TRACE;
    if (strcmp(yytext, "Solicit") == 0) {
        yylval.v_uint = 1; //DH6_SOLICIT;
    } else if (strcmp(yytext, "Advertise") == 0) {
        yylval.v_uint = 2;
    } else if (strcmp(yytext, "Request") == 0) {
        yylval.v_uint = 3;
    } else if (strcmp(yytext, "Confirm") == 0) {
        yylval.v_uint = 4;
    } else if (strcmp(yytext, "Renew") == 0) {
        yylval.v_uint = 5;
    } else if (strcmp(yytext, "Rebind") == 0) {
        yylval.v_uint = 6;
    } else if (strcmp(yytext, "Reply") == 0) {
        yylval.v_uint = 7;
    } else if (strcmp(yytext, "Release") == 0) {
        yylval.v_uint = 8;
    } else if (strcmp(yytext, "Reconfigure") == 0) {
        yylval.v_uint = 9;
    } else if (strcmp(yytext, "Info-Request") == 0) {
        yylval.v_uint = 10;
    } else if (strcmp(yytext, "Relay-Forward") == 0) {
        yylval.v_uint = 11;
    } else if (strcmp(yytext, "Relay-Reply") == 0) {
        yylval.v_uint = 12;
    }

    return V_DH6TYPE;
}

ia-id       { TRACE; return IAID; }
t1          { TRACE; return T1; }
t2          { TRACE; return T2; }
ia-addr     { TRACE; return IAADDR; }
status-code { TRACE; BEGIN S_STATUS; return STATUSCODE; }

address     { TRACE; return ADDRESS; }
preferred-lifetime   { TRACE; return PRFLFT; }
valid-lifetime  { TRACE; return VLDLFT; }

duid-type   { TRACE; return DUIDTYPE; }
hw-type     { TRACE; return HWTYPE; }
ll-addr     { TRACE; return LLADDR; }
(duid-llt|duid-en|duid-ll) {
    TRACE;
    if (strcmp(yytext, "duid-llt") == 0)
        yylval.v_uint = 1;
    else if (strcmp(yytext, "duid-en") == 0)
        yylval.v_uint = 2;
    else if (strcmp(yytext, "duid-ll") == 0)
        yylval.v_uint = 3;

    return V_DUIDTYPE;
}

ethernet  { 
    TRACE;
    if (strcmp(yytext, "ethernet") == 0) {
        yylval.v_uint = 1;
    } else {
        yyerror("invalid hardware type %s\n", yytext);
    }

    return V_HWTYPE;
}

    /* values */
[0-9]+ {  
    TRACE;
    yylval.v_uint = atoi(yytext);
    return V_UINT;
}

0x{hexdigit}+ { 
    TRACE;
    sscanf(yytext, "0x%x", &yylval.v_uint);
    return V_HEX; 
}

\"[^"]*\" { 
    TRACE;
    yylval.v_str = strdup(yytext + 1);
    if (yyleng <= 2 || yylval.v_str[yyleng - 2] != '"')
        yyerror("improperly terminated string\n");
    else
        yylval.v_str[yyleng - 2] = 0;
    return V_STR;
}

{hexpair}:{hexpair}:{hexpair}:{hexpair}:{hexpair}:{hexpair} {
    TRACE;
    sscanf(yytext, "%02x:%02x:%02x:%02x:%02x", 
            &yylval.v_mac[0], &yylval.v_mac[1], &yylval.v_mac[2], 
            &yylval.v_mac[3], &yylval.v_mac[4], &yylval.v_mac[5]);
    return V_MAC;
}

code        { TRACE; return CODE; }
message     { TRACE; return MESSAGE; }

{hexdigit}{1,4}:[0-9a-fA-F:]+  {
    TRACE;
    if (inet_pton(AF_INET6, yytext, &yylval.v_addr6) <= 0)
        yyerror("invalid IPv6 address: %s\n", yytext);
    return V_ADDR6;
}

[ \n\t\r]+  ; /* Ignore white spaces */
.       { TRACE; yyerror("invalid token: `%s'\n", yytext); }

%%

int yywrap(void)
{
    return 1;
}
