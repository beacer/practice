#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/netfilter.h>
#include <libnetfilter_queue/libnetfilter_queue.h>

struct tcp_opt {
    unsigned char kind;
    unsigned char len;
    unsigned char val[0];
};

struct tcp_opt_ts {
    struct tcp_opt hdr;
    u_int32_t ts_val;
    u_int32_t ts_erc;
};

static u_int16_t checksum(u_int32_t init, u_int8_t *addr, size_t count)
{
    /* Compute Internet Checksum for "count" bytes
     * beginning at location "addr".
     */
    u_int32_t sum = init;

    while( count > 1 ) {
        /* This is the inner loop */
        sum += ntohs(* (u_int16_t*) addr);
        addr += 2;
        count -= 2;
    }

    /* Add left-over byte, if any */
    if( count > 0 )
        sum += * (u_int8_t *) addr;

    /* Fold 32-bit sum to 16 bits */
    while (sum>>16)
        sum = (sum & 0xffff) + (sum >> 16);

    return (u_int16_t)~sum;
}

static void set_ip_checksum(struct iphdr* iphdrp)
{
    iphdrp->check = 0;
    iphdrp->check = htons(checksum(0, (u_int8_t*)iphdrp, iphdrp->ihl<<2));
}

static u_int16_t tcp_checksum2(struct iphdr* iphdrp, struct tcphdr* tcphdrp)
{
    size_t tcplen = ntohs(iphdrp->tot_len) - (iphdrp->ihl<<2);
    u_int32_t cksum = 0;

    cksum += ntohs((iphdrp->saddr >> 16) & 0x0000ffff);
    cksum += ntohs(iphdrp->saddr & 0x0000ffff);
    cksum += ntohs((iphdrp->daddr >> 16) & 0x0000ffff);
    cksum += ntohs(iphdrp->daddr & 0x0000ffff);
    cksum += iphdrp->protocol & 0x00ff;
    cksum += tcplen;
    return checksum(cksum, (u_int8_t*)tcphdrp, tcplen);
}

static void set_tcp_checksum2(struct iphdr* iphdrp, struct tcphdr* tcphdrp)
{
    tcphdrp->check = 0;
    tcphdrp->check = htons(tcp_checksum2(iphdrp, tcphdrp));
}

static void set_tcp_checksum1(struct iphdr* iphdrp)
{
    struct tcphdr *tcphdrp = 
        (struct tcphdr*)((u_int8_t*)iphdrp + (iphdrp->ihl<<2));

    set_tcp_checksum2(iphdrp, tcphdrp);
}

static void hex_dump(unsigned char *start, int size)
{
    int i;

    printf("  ");
    for (i = 0; i < size; i++) {
        if (i % 16 == 0 && i != 0)
            printf("\n  ");
        if (i % 8 == 0 && i != 0 && i % 16 != 0)
            printf(" ");
        printf("%02x ", start[i]);
    }
    printf("\n");
}

static void dump_ip_hdr(struct iphdr *iph)
{
    char saddr[16], daddr[16];

    if (!iph)
        return;

    printf("[IP] ver %u ihl %u tos %u tot %u ", 
            iph->version, iph->ihl<<2, iph->tos, ntohs(iph->tot_len));
    printf("id %u frag_off %04x ttl %u prot %u chksum %04x ", 
            ntohs(iph->id), ntohs(iph->frag_off), iph->ttl, 
            iph->protocol, ntohs(iph->check));
    printf("src %s dst %s\n", 
            inet_ntop(AF_INET, &iph->saddr, saddr, sizeof(saddr)), /* won't error */
            inet_ntop(AF_INET, &iph->daddr, daddr, sizeof(daddr)));

    return;
}

static void dump_tcp_hdr(struct tcphdr *tcph)
{
    printf("[TCP] sport %u dport %u seq %u ack %u doff %u <", 
            ntohs(tcph->source), ntohs(tcph->dest), ntohl(tcph->seq), 
            ntohl(tcph->ack), tcph->doff);
    if (tcph->fin)
        printf("FIN ");
    if (tcph->syn)
        printf("SYN ");
    if (tcph->ack)
        printf("ACK ");
    if (tcph->psh)
        printf("PUSH ");
    if (tcph->rst)
        printf("RST ");
    printf("> window %u chksum %04x\n", ntohs(tcph->window), ntohs(tcph->check));
}

enum {
    TS_UNCHANGED,
    TS_INSERTED,
};

static int try_insert_tcp_syn_ts(struct iphdr *iph, struct tcphdr *tcph, 
        unsigned char **payload, u_int32_t *len)
{
    unsigned char *ptr;
    int i, new_len;
    struct tcp_opt *opt;
    struct tcp_opt_ts *ts_opt;

    if (!tcph->syn)
        return TS_UNCHANGED;

#if 0
    if ((ptr = malloc(*len + 4)) == NULL) {
        fprintf(stderr, "%s: no memory\n", __FUNCTION__);
        return TS_UNCHANGED;
    }

    memcpy(ptr, *payload, *len);
    *payload = ptr;
    (*len) += 4;
    iph = (struct iphdr *)(*payload);
    tcph = (struct tcphdr *)(*payload + (iph->ihl<<2));

    ptr = (unsigned char *)tcph + tcph->doff * 4;
    *ptr++ = 1; // nop
    *ptr++ = 1; // nop
    *ptr++ = 1; // nop
    *ptr++ = 1; // nop
    tcph->doff++;

    iph->tot_len = htons(ntohs(iph->tot_len) + 4);
#else
    /*
     * tcph->doff indicates where TCP data begins,
     * however the option "end-of-option-list" MAY also be used to 
     * indicate the end of option.
     */
    ptr = (unsigned char *)(tcph + 1); /* skip TCP basic header */
    for (i = 0; i < tcph->doff * 4 - sizeof(struct tcphdr); i++) {
        opt = (struct tcp_opt *)(ptr + i);

        if (opt->kind == 0x00) // end-of-option-list
            break;
        if (opt->kind == 0x01) // NOP
            continue;
        if (opt->kind == 0x08) { // timestamps
            fprintf(stderr, "alreay have TS option !\n");
            return TS_UNCHANGED;
        }

        ptr += opt->len - 1;
    }

    new_len = *len + sizeof(struct tcp_opt_ts) + 2; // 2 for NOP
    if (new_len % 4)
        new_len = (new_len + 4) / 4 * 4; // never goes here

    if ((ptr = malloc(new_len)) == NULL) {
        fprintf(stderr, "%s: no memory\n", __FUNCTION__);
        return TS_UNCHANGED;
    }

    /* make sure all new space (after EOO) as NOP */
    memset(ptr, 0x01, new_len);
    /* copy old TCP header */
    memcpy(ptr, *payload, *len);

    /* set timestamps option */
    ts_opt = (struct tcp_opt_ts *)(ptr + *len);
    ts_opt->hdr.kind = 0x08;
    ts_opt->hdr.len = 10;
    ts_opt->ts_val = htonl(time(NULL));
    ts_opt->ts_erc = htonl(time(NULL));

    *payload = ptr;

    iph = (struct iphdr *)(*payload);
    tcph = (struct tcphdr *)(*payload + (iph->ihl<<2));

    tcph->doff += (new_len - *len) / 4;
    iph->tot_len = htons(ntohs(iph->tot_len) + new_len - *len);
    (*len) = new_len;

    ... ...
    2 byte garbage after TS option
    ip->tot_len not checked
    tcp->doff not checked
    recheck all fields of IP / TCP
    ... ...
    /**/
#endif

    /* re-calculate IP and TCL checksum */
    set_ip_checksum(iph);
    set_tcp_checksum1(iph);

    return TS_INSERTED;
}

static int tcp_add_syn_ts(struct nfq_q_handle *qh, struct nfgenmsg *nfmsg, 
        struct nfq_data *nfad, void *data)
{
    u_int32_t pkt_id = 0, mark, ifi;
    struct nfqnl_msg_packet_hdr *ph;
    struct nfqnl_msg_packet_hw *hwph;
    u_int32_t payload_len;
    unsigned char *payload;
    struct iphdr *iph;
    struct tcphdr *tcph;
    int i, hlen, debug = (int)data;
    int verdict;

    if (debug)
        printf(">>> New Packet\n");

    if ((ph = nfq_get_msg_packet_hdr(nfad)) != NULL) {
        pkt_id = ntohl(ph->packet_id);
        /*if (debug) {
            printf("ptype 0x%04x hook %u id %u\n", 
                    ntohs(ph->hw_protocol), ph->hook, pkt_id);
        }*/
    }

    if (debug) {
        /* L2 header & device */
        printf("[L2] ");
        if ((hwph = nfq_get_packet_hw(nfad)) != NULL) {
            hlen = ntohs(hwph->hw_addrlen);

            printf("smac ");
            for (i = 0; i < hlen - 1; i++)
                printf("%02x:", hwph->hw_addr[i]);
            printf("%02x ", hwph->hw_addr[hlen - 1]);
        }

        if ((mark = nfq_get_nfmark(nfad)) != 0)
            printf("nfmark %u ", mark);
        if ((ifi = nfq_get_indev(nfad)) != 0)
            printf("indev %u ", ifi);
        if ((ifi = nfq_get_outdev(nfad)) != 0)
            printf("outdev %u ", ifi);
        if ((ifi = nfq_get_physindev(nfad)) != 0)
            printf("phyindev %u ", ifi);
        if ((ifi = nfq_get_physoutdev(nfad)) != 0)
            printf("phyoutdev %u ", ifi);
    }

    payload_len = nfq_get_payload(nfad, &payload);
    if (payload_len < sizeof(struct iphdr)) {
        fprintf(stderr, "%s: invild payload length\n", __FUNCTION__);
        goto accept;
    }
    if (debug)
        printf("payload length: %d\n", payload_len);

    iph = (struct iphdr *)payload;
    if (debug)
        dump_ip_hdr(iph);

    if (iph->protocol != IPPROTO_TCP 
            || iph->tot_len < ((iph->ihl<<2) + sizeof(struct tcphdr))) {
        //fprintf(stderr, "%s: not TCP packet\n", __FUNCTION__);
        goto accept;
    }

    tcph = (struct tcphdr *)(payload + (iph->ihl<<2));
    if (debug)
        dump_tcp_hdr(tcph);

    /* 
     * adding the timestamp if not exist, 
     * and re-calulate both IP and TCP checksum if need.
     */
    if (try_insert_tcp_syn_ts(iph, tcph, &payload, &payload_len) == TS_UNCHANGED)
        goto accept;

    if (debug) {
        printf("[MOD] TS inserted, payload len: %d\n", payload_len);
        hex_dump(payload, payload_len);
    }

    /* XXX: we just allocated now buffer for payload, will the old buf leak ? 
     * need check the Kernel code since no enough doc */
    verdict = nfq_set_verdict2(qh, pkt_id, NF_REPEAT, 1, payload_len, payload);
    free(payload);
    return verdict;

accept:
    return nfq_set_verdict(qh, pkt_id, NF_ACCEPT, 0, NULL);
}

int main(int argc, char *argv[])
{
    struct nfq_handle *h;
    struct nfq_q_handle *qh;
    struct nfnl_handle *nlh;
    int qid = 0, nlfd, nrcv;
    char buf[4096];
    int debug = 1;

    if ((h = nfq_open()) == NULL) {
        fprintf(stderr, "nfq_open() error\n");
        exit(1);
    }

    if (nfq_unbind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "nfq_unbind_pf() error\n");
        exit(1);
    }

    if (nfq_bind_pf(h, AF_INET) < 0) {
        fprintf(stderr, "nfq_unbind_pf() error\n");
        exit(1);
    }

    if (argc >= 2)
        qid = atoi(argv[1]);
    fprintf(stderr, "binding to nfq queue %d\n", qid);

    if (argc == 3)
        debug = atoi(argv[2]);

    /* one queue for TCP timestamps option check and adding,
     * use more queues if need */
    if ((qh = nfq_create_queue(h, qid, &tcp_add_syn_ts, (void *)debug)) == NULL) {
        fprintf(stderr, "nfq_create_queue() error\n");
        exit(1);
    }

    if (nfq_set_mode(qh, NFQNL_COPY_PACKET, 0xffff) < 0) {
        fprintf(stderr, "nfq_set_mode () error\n");
        exit(1);
    }

    if ((nlh = nfq_nfnlh(h)) == NULL) {
        fprintf(stderr, "nfq_nfnlh() error\n");
        exit(1);
    }
    if ((nlfd = nfnl_fd(nlh)) < 0) {
        fprintf(stderr, "nfnl_fd() error\n");
        exit(1);
    }

    while ((nrcv = recv(nlfd, buf, sizeof(buf), 0)) >= 0) {
        nfq_handle_packet(h, buf, nrcv);
    }

    fprintf(stderr, "exit nfq handling: %s\n", strerror(errno));

    nfq_destroy_queue(qh);
    nfq_close(h);
    exit(0);
}



