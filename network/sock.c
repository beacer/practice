/*
 * simple socket server/client to test TCP/UDP over IPv4/IPv6.
 * support getaddrinfo() and SO_BINDTODEVICE (Linux).
 *
 * Lei Chen
 * May 2015 - initial
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <netdb.h>
#include <stdarg.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#ifndef NELEMS
#define NELEMS(arr)         (sizeof(arr) / sizeof((arr)[0]))
#endif

#define LISTENQ             15
#define MAXSOCK             32
#define MAXBUFSZ            1500

/* global configs */
static bool debug_mode = false;
static int log_thresh = LOG_WARNING;

static void usage(void)
{
    fprintf(stderr, "Usage:\n");
    fprintf(stderr, "    sock [OPTIONS] HOSTNAME SERVICE\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "    -s             work as server (default)\n");
    fprintf(stderr, "    -c             work as client\n");
    fprintf(stderr, "    -t socktype    socket is \"tcp\" (default), \"udp\"\n");
#ifdef SO_BINDTODEVICE
    fprintf(stderr, "    -b add[:port]  bind to local address and/or port\n");
    fprintf(stderr, "                   it's a client ONLY option\n");
#endif
    fprintf(stderr, "    -i interface   bind to interface\n");
    fprintf(stderr, "    -4             use IPv4 socket\n");
    fprintf(stderr, "    -6             use IPv6 socket\n");
    fprintf(stderr, "    -h             show this help message\n");
    fprintf(stderr, "    -d             debug mode (logging to stderr)\n");
    fprintf(stderr, "    -l level       logging thread 0~7 (default is 4: LOG_WARNING)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, "    HOSTNAME       hostname or ADDRESS\n");
    fprintf(stderr, "    SERVICE        service or port number\n");
    fprintf(stderr, "    ADDRESS        IPv4/IPv6 address or empty string (wildcard)\n");
}

static inline void DPRINT(int level, const char *fmt, ...)
{
    va_list ap;
    char buf[MAXBUFSZ];
    char *cp;

    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if ((cp = strrchr(buf, '\n')) != NULL)
        *cp = '\0';

    if (debug_mode && level <= log_thresh) {
        fprintf(stderr, "<%d> %s\n", level, buf);
    } else {
        syslog(level, "%s", buf);
    }

    return;
}

struct skconf {
    bool isserv;
    int family;         /* AF_XXX */
    int socktype;       /* socket type, SOCK_STREAM, SOCK_DGRAM, SOCK_RAW  */
    int protocol;       /* for RAW socket, IPPROTO_XXX */

    char *hostname;     /* server */
    char *service;      /* local address */

#ifdef SO_BINDTODEVICE
    char *ifname;       /* interface to bind */
#endif
    char *saddr;        /* client only, bind to local addr */
    char *sport;        /* client only, bind to local port */
};

struct sock {
    int fd;
    int family;
    int socktype;
    int protocol;

    /* 
     * do we need buffer ?
     * o if we consider write may block (sockfd are set NON-Block) 
     *   or send bytes less then we want,
     *   we use slect for write, and send as many as we can and left 
     *   unsend in buffer
     * o as a client it become more complecated, two buffers needed
     *   stdin  -> buffer -> sockfd 
     *   sockfd -> buffer -> stdout 
     * 
     * To make things easier, we do not use buffer
     * o for TCP so we assume write() won't got block, and we use writen to 
     *   ensure the all data sent
     * o for udp it's dgram based, sendto entire datagram been sent out
     *
     * Note it still has 'block' or write less data issue
     * we either use buffers or seperated threads/processes to handle it.
     * see [UNPv1e3 6.5-6.7, 16.2, 26.3] for more discussion.
     */
};

ssize_t                     /* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
    size_t      nleft;
    ssize_t     nwritten;
    const char  *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;       /* and call write() again */
            else
                return(-1);         /* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }
    return(n);
}

static void free_conf(struct skconf *cf)
{
    if (!cf)
        return;

    if (cf->hostname)
        free(cf->hostname);
    if (cf->service)
        free(cf->service);
#ifdef SO_BINDTODEVICE
    if (cf->ifname)
        free(cf->ifname);
#endif
    if (cf->saddr)
        free(cf->saddr);
    if (cf->sport)
        free(cf->sport);

    free(cf);
    return;
}

static struct skconf *load_conf(int argc, char * const argv[])
{
    struct skconf *cf;
    int opt;
    char *cp;

    if ((cf = calloc(1, sizeof(struct skconf))) == NULL) {
        DPRINT(LOG_ERR, "%s: no memory\n", __FUNCTION__);
        return NULL;
    }

    /* default values */
    cf->isserv = true;
    cf->family = AF_UNSPEC;
    cf->socktype = 0; /* use SOCK_STREAM and/or SOCK_DGRAM */

    while ((opt = getopt(argc, argv, "sct:b:i:46hdl:")) != -1) {
        switch (opt) {
        case 's':
            cf->isserv = true;
            break;
        case 'c':
            DPRINT(LOG_ERR, "%s: client mode not support now\n", __FUNCTION__);
            free(cf);
            return NULL;
            //cf->isserv = false;
            break;
        case 't':
            if (strcasecmp(optarg, "tcp") == 0) {
                cf->socktype = SOCK_STREAM;
            } else if (strcasecmp(optarg, "udp") == 0) {
                cf->socktype = SOCK_DGRAM;
            } else {
                DPRINT(LOG_ERR, "%s: protocol not support: %s\n", __FUNCTION__, optarg);
                free(cf);
                return NULL;
            }
            break;
        case 'b':
            /* clone first, not sure if optarg is writable */
            cf->saddr = strdup(optarg);
            if ((cp = strrchr(cf->saddr, ':')) != NULL) {
                *cp++ = '\0';
                cf->sport = strdup(cp);
            }
            break;
#ifdef SO_BINDTODEVICE
        case 'i':
            cf->ifname = strdup(optarg);
            break;
#endif
        case '4':
            cf->family = AF_INET;
            break;
        case '6':
            cf->family = AF_INET6;
            break;
        case 'h':
            usage();
            exit(0);
        case 'd':
            debug_mode = true;
            break;
        case 'l':
            if (atoi(optarg) >= LOG_EMERG && atoi(optarg) <= LOG_DEBUG)
                log_thresh = atoi(optarg);
            break;
        case '?':
        default:
            DPRINT(LOG_ERR, "%s: invalid argument `%c'\n", __FUNCTION__, opt);
            free(cf);
            return NULL;
        }
    }

    argc -= optind;
    argv += optind;

    if (argc < 2) {
        usage();
        exit(1);
    }

    if (strlen(argv[0]))
        cf->hostname = strdup(argv[0]);
    if (strlen(argv[1]))
        cf->service = strdup(argv[1]);

    DPRINT(LOG_DEBUG, "[Configurations]");
    DPRINT(LOG_DEBUG, "  Server   %s", cf->isserv ? "yes" : "no");
    DPRINT(LOG_DEBUG, "  Family   %d", cf->family);
    DPRINT(LOG_DEBUG, "  Protocol %d", cf->protocol);
    DPRINT(LOG_DEBUG, "  Hostname %s", cf->hostname);
    DPRINT(LOG_DEBUG, "  Service  %s", cf->service);
#ifdef SO_BINDTODEVICE
    if (cf->ifname)
        DPRINT(LOG_DEBUG, "  Interface  %s", cf->ifname);
#endif
    if (cf->saddr)
        DPRINT(LOG_DEBUG, "  Local Address  %s", cf->saddr);
    if (cf->sport)
        DPRINT(LOG_DEBUG, "  Local Port  %s", cf->sport);

    return cf;
}

static int fd_set_flag(int fd, int flag)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL, 0)) < 0)
        return -1;
    if (fcntl(fd, F_SETFL, flags | flag) < 0)
        return -1;

    return 0;
}

static void dump_sockaddr(const char *pref, const struct sockaddr *sa)
{
    struct sockaddr_in *sin;
    struct sockaddr_in6 *sin6;
    char addrbuf[64];

    if (!pref)
        pref = "";

    switch (sa->sa_family) {
    case AF_INET:
        sin = (struct sockaddr_in *)sa;
        if (inet_ntop(AF_INET, &sin->sin_addr, addrbuf, sizeof(addrbuf)) != NULL)
            DPRINT(LOG_DEBUG, "%sai.ipv4 %s", pref, addrbuf);
        DPRINT(LOG_DEBUG, "%sai.port %d", pref, ntohs(sin->sin_port));
        break;
    case AF_INET6:
        sin6 = (struct sockaddr_in6 *)sa;
        if (inet_ntop(AF_INET6, &sin6->sin6_addr, addrbuf, sizeof(addrbuf)) != NULL)
            DPRINT(LOG_DEBUG, "%sai.ipv6 %s", pref, addrbuf);
        DPRINT(LOG_DEBUG, "%sai.port %d", pref, ntohs(sin6->sin6_port));
        break;
    default:
        break;
    }

    return;
}

static void dump_addrinfo(const struct addrinfo *ai)
{
    if (!ai)
        return;

    DPRINT(LOG_DEBUG, "[AddrInfo]");
    DPRINT(LOG_DEBUG, "  ai.ai_flags %d", ai->ai_flags);
    DPRINT(LOG_DEBUG, "  ai.ai_family %d", ai->ai_family);
    DPRINT(LOG_DEBUG, "  ai.ai_socktype %d", ai->ai_socktype);
    DPRINT(LOG_DEBUG, "  ai.ai_protocol %d", ai->ai_protocol);
    DPRINT(LOG_DEBUG, "  ai.ai_canonname %s", ai->ai_canonname ? ai->ai_canonname : "<none>");
    dump_sockaddr("  ", ai->ai_addr);

    return;
}

/* 
 * create socket fd(s) with config. As a server more than 
 * one socket may created e.g., for TCP/UDP, different families.
 *
 * @socks [out] the buffer to save opened sock{}s.
 * @nsock [in-out] the max number of sock{} as input 
 *     and real sock{} numbers as output.
 *     sock{} could be more then 1 only for a server.
 */
static int sock_create(const struct skconf *cf, struct sock socks[], size_t *nsock)
{
    int err, off;
    struct sock *sk;
    struct addrinfo hint, *results, *ai;
    const int on = 1;
    struct sockaddr_in sin;
    struct sockaddr_in6 sin6;
    struct sockaddr *sa;
#ifdef SO_BINDTODEVICE
    struct ifreq ifr;
#endif

    assert(cf && socks && nsock);

    memset(&hint, 0, sizeof(hint));
    if (cf->isserv)
        hint.ai_flags |= AI_PASSIVE;
    hint.ai_family = cf->family;
    hint.ai_socktype = cf->socktype;
    hint.ai_protocol = cf->protocol;

    if ((err = getaddrinfo(cf->hostname, cf->service, &hint, &results)) != 0) {
        DPRINT(LOG_ERR, "%s: getaddrinfo: %s\n", __FUNCTION__, gai_strerror(err));
        return -1;
    }

    off = 0;
    for (ai = results; ai != NULL; ai = ai->ai_next) {
        dump_addrinfo(ai);

        if (off >= *nsock) {
            DPRINT(LOG_INFO, "%s: no space for new fd", __FUNCTION__);
            break;
        }

        sk = &socks[off];

        if ((sk->fd = socket(ai->ai_family, ai->ai_socktype, ai->ai_protocol)) <= 0) {
            DPRINT(LOG_ERR, "%s: socket: %s\n", __FUNCTION__, strerror(errno));
            continue;
        }
        sk->family = ai->ai_family;
        sk->socktype = ai->ai_socktype;
        sk->protocol = ai->ai_protocol;

        if (cf->isserv) {
            if (setsockopt(sk->fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) != 0) {
                DPRINT(LOG_ERR, "%s: setsockopt: %s\n", __FUNCTION__, strerror(errno));
                close(sk->fd);
                continue;
            }

            if (bind(sk->fd, ai->ai_addr, ai->ai_addrlen) != 0) {
                DPRINT(LOG_ERR, "%s: bind: %s\n", __FUNCTION__, strerror(errno));
                close(sk->fd);
                continue;
            }

            if (ai->ai_socktype == SOCK_STREAM && listen(sk->fd, LISTENQ) != 0) {
                DPRINT(LOG_ERR, "%s: setsockopt: %s\n", __FUNCTION__, strerror(errno));
                close(sk->fd);
                continue;
            }
        } else { /* client */
            if (cf->saddr || cf->sport) {
                switch (ai->ai_family) {
                case AF_INET:
                    sin.sin_family = AF_INET;
                    if (!cf->saddr || !strlen(cf->saddr)) {
                        sin.sin_addr.s_addr = htonl(INADDR_ANY);
                    } else {
                        if (inet_pton(ai->ai_family, cf->saddr, &sin.sin_addr) <= 0) {
                            DPRINT(LOG_ERR, "%s: bad address: %s\n", __FUNCTION__, cf->saddr);
                            sin.sin_addr.s_addr = htonl(INADDR_ANY);
                        }
                    }

                    if (!cf->sport || !strlen(cf->sport)) {
                        sin.sin_port = 0;
                    } else {
                        sin.sin_port = htons(atoi(cf->sport));
                    }

                    sa = (struct sockaddr *)&sin;
                    break;
                case AF_INET6:
                    sin6.sin6_family = AF_INET6;
                    if (!cf->saddr || !strlen(cf->saddr)) {
                        sin6.sin6_addr = in6addr_any;
                    } else {
                        if (inet_pton(ai->ai_family, cf->saddr, &sin6.sin6_addr) <= 0) {
                            DPRINT(LOG_ERR, "%s: bad address: %s\n", __FUNCTION__, cf->saddr);
                            sin6.sin6_addr = in6addr_any;
                        }
                    }

                    if (!cf->sport || !strlen(cf->sport)) {
                        sin6.sin6_port = 0;
                    } else {
                        sin6.sin6_port = htons(atoi(cf->sport));
                    }

                    sa = (struct sockaddr *)&sin6;
                    break;
                default:
                    DPRINT(LOG_ERR, "%s: bad family to bind\n", __FUNCTION__);
                    break;
                }

                if (bind(sk->fd, sa, sizeof(struct sockaddr)) != 0) {
                    DPRINT(LOG_ERR, "%s: bind (client): %s\n", __FUNCTION__, strerror(errno));
                    close(sk->fd);
                    continue;
                }
            }

            if (connect(sk->fd, ai->ai_addr, ai->ai_addrlen) != 0) {
                DPRINT(LOG_ERR, "%s: setsockopt: %s\n", __FUNCTION__, strerror(errno));
                close(sk->fd); /* sockfd cannot be re-used if fail to connect */
                continue;
            }

            break; /* connect one of the server success */
        }

#ifdef SO_BINDTODEVICE
        if (cf->ifname) {
            memset(&ifr, 0, sizeof(ifr));
            snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", cf->ifname);
            if (setsockopt(sk->fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) != 0) {
                DPRINT(LOG_ERR, "%s: fail to bind iface: %s\n", __FUNCTION__, cf->ifname);
                close(sk->fd);
                continue;
            }
        }
#endif

        if (fd_set_flag(sk->fd, O_NONBLOCK) != 0) {
            DPRINT(LOG_ERR, "%s: fail to set O_NONBLOCK: %s\n", __FUNCTION__, strerror(errno));
            close(sk->fd);
            continue;
        }

        off++;
    }

    *nsock = off;
    freeaddrinfo(results);
    return 0;
}

static void load_fdset(struct sock socks[], int nsock, fd_set *fdset, int *maxfd)
{
    int i;

    for (i = 0; i < nsock; i++) {
        if (socks[i].fd <= 0)
            continue;

        FD_SET(socks[i].fd, fdset);
        if (*maxfd <= socks[i].fd)
            *maxfd = socks[i].fd + 1;
    }
}

static int tcp_accept_conn(int family, int listenfd)
{
    int connfd = -1;
    struct sockaddr_in cliaddr;
    struct sockaddr_in6 cliaddr6;
    struct sockaddr *sa;
    socklen_t addrlen;

    switch (family) {
    case AF_INET:
        addrlen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &addrlen);
        sa = (struct sockaddr *)&cliaddr;
        break;
    case AF_INET6:
        addrlen = sizeof(cliaddr6);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr6, &addrlen);
        sa = (struct sockaddr *)&cliaddr6;
        break;
    default:
        DPRINT(LOG_ERR, "%s: bad family: %d\n", __FUNCTION__, family);
        exit(1);
    }

    if (connfd < 0) {
        if (errno == EWOULDBLOCK
                || errno == ECONNABORTED
                || errno == EPROTO
                || errno == EINTR)
            return -1;

        DPRINT(LOG_ERR, "%s: accept: %s\n", __FUNCTION__, strerror(errno));
        exit(1);
    }

    DPRINT(LOG_DEBUG, "[New Connection]");
    dump_sockaddr("  ", sa);

    return connfd;
}

static int tcp_handle_req(int fd)
{
    char buf[MAXBUFSZ];
    int n;

    if ((n = read(fd, buf, sizeof(buf))) <= 0) {
        if (n < 0)
            DPRINT(LOG_ERR, "%s: read error %s\n", __FUNCTION__, strerror(errno));
        else if (n == 0)
            DPRINT(LOG_ERR, "%s: EOF !\n", __FUNCTION__);

        return -1;
    }

    if (writen(fd, buf, n) != n) {
        DPRINT(LOG_ERR, "%s: write error %s\n", __FUNCTION__, strerror(errno));
        return -1;
    }

    return 0;
}

static int udp_handle_req(int fd, int family)
{
    char buf[MAXBUFSZ];
    struct sockaddr_in cliaddr;
    struct sockaddr_in6 cliaddr6;
    struct sockaddr *sa;
    socklen_t addrlen;
    int n;

    switch (family) {
        case AF_INET:
            addrlen = sizeof(cliaddr);
            n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr, &addrlen);

            sa = (struct sockaddr *)&cliaddr;
            break;
        case AF_INET6:
            addrlen = sizeof(cliaddr6);
            n = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&cliaddr6, &addrlen);

            sa = (struct sockaddr *)&cliaddr6;
            break;
        default:
            return -1;
    }

    if (n < 0) {
        DPRINT(LOG_ERR, "%s: read error %s\n", __FUNCTION__, strerror(errno));
        return -1;
    } else if (n > 0) {
        if (sendto(fd, buf, n, 0, sa, addrlen) < 0)
            DPRINT(LOG_ERR, "%s: send error %s\n", __FUNCTION__, strerror(errno));
    }

    return 0;
}

int main(int argc, char *argv[])
{
    const char *prog;
    struct skconf *cf;
    struct sock socks[MAXSOCK], csocks[MAXSOCK], *sk;
    int maxfd, i, j, connfd;
    size_t nsk, ncsk;
    fd_set rset;

    if ((prog = strrchr(argv[0], '/')) != NULL)
        prog++;
    else
        prog = argv[0];

    if ((cf = load_conf(argc, argv)) == NULL)
        exit(1);

    nsk = NELEMS(socks);
    if (sock_create(cf, socks, &nsk) != 0 || nsk == 0) {
        DPRINT(LOG_ERR, "%s: no sock created\n", prog);
        exit(1);
    }

    ncsk = 0;
    for (i = 0; i < NELEMS(csocks); i++)
        csocks[i].fd = -1;

    while (1) {
        FD_ZERO(&rset);
        maxfd = 0;
        load_fdset(socks, nsk, &rset, &maxfd);
        load_fdset(csocks, ncsk, &rset, &maxfd);

        if (select(maxfd, &rset, NULL, NULL, NULL) <= 0) {
            if (errno == EINTR)
                continue;
            DPRINT(LOG_ERR, "%s: select: %s\n", prog, strerror(errno));
            exit(1);
        }

        for (i = 0; i < nsk; i++) {
            sk = &socks[i];

            if (!FD_ISSET(sk->fd, &rset))
                continue;

            if (cf->isserv) {
                if (sk->socktype == SOCK_STREAM) { /* TCP Server (listen) */
                    if ((connfd = tcp_accept_conn(sk->family, sk->fd)) == -1)
                        continue;

                    for (j = 0; j < NELEMS(csocks); j++) {
                        if (csocks[j].fd >= 0)
                            continue;

                        csocks[j].fd = connfd;
                        csocks[j].family = sk->family;
                        csocks[j].socktype = sk->socktype;
                        csocks[j].protocol = sk->protocol;
                        break;
                    }
                    if (j == NELEMS(csocks)) {
                        DPRINT(LOG_ERR, "%s: no space for new connection\n", prog);
                        close(connfd);
                        continue;
                    } else if (j >= ncsk) {
                        ncsk = j + 1;
                    }
                } else if (sk->socktype == SOCK_DGRAM) { /* UDP Server*/
                    if (udp_handle_req(sk->fd, sk->family) != 0)
                        continue;
                }
            } else { /* client */
                // TODO:
            }
        }

        /* TCP Server (connection)  */
        for (i = 0; i < ncsk; i++) {
            sk = &csocks[i];
            if (sk->fd < 0 || !FD_ISSET(sk->fd, &rset))
                continue;

            if (tcp_handle_req(sk->fd) != 0) {
                close(sk->fd);
                sk->fd = -1;
            }
        }
    }

    for (i = 0; i < nsk; i++)
        if (socks[i].fd >= 0)
            close(socks[i].fd);
    for (i = 0; i < ncsk; i++)
        if (csocks[i].fd >= 0)
            close(csocks[i].fd);
    free_conf(cf);

    exit(0);
}
