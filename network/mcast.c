/*
 * mcast.c add/leave an multicast group (or SSM group).
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

static int
get_if_index(int sockfd, const char *ifname)
{
    struct ifreq ifr;

    bzero(&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name) - 1);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
        return -1;

    return (int)ifr.ifr_ifindex;
}

static int
addr_str_to_struct(const char *straddr, int port, 
        struct sockaddr *sockaddr, int size)
{
    struct sockaddr_in in_addr;

    bzero(&in_addr, sizeof(in_addr));
    in_addr.sin_family = AF_INET;
    in_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, straddr, 
                (struct sockaddr *)&in_addr.sin_addr) <= 0)
        return -1;

    if (size < sizeof(in_addr))
        return -1;

    memcpy(sockaddr, &in_addr, sizeof(in_addr));

    return 0;
}

int
mcast_join(int sockfd, const char *multiaddr, const char *ifname)
{
    struct group_req greq;

    if (sockfd < 0 || multiaddr == NULL)
        return -1;

    if (ifname == NULL)
        greq.gr_interface = 0;
    else if ((greq.gr_interface = get_if_index(sockfd, ifname)) == -1)
        return -1;

    if (addr_str_to_struct(multiaddr, 0, 
                (struct sockaddr *)&greq.gr_group, 
                sizeof(greq.gr_group)) == -1)
        return -1;

    return setsockopt(sockfd, IPPROTO_IP, MCAST_JOIN_GROUP, 
            &greq, sizeof(greq));
}

int 
mcast_leave(int sockfd, const char *multiaddr)
{
    struct group_req greq;

    bzero(&greq, sizeof(greq));
    greq.gr_interface = 0;
    if (addr_str_to_struct(multiaddr, 0, 
                (struct sockaddr *)&greq.gr_group, 
                sizeof(greq.gr_group)) == -1)
        return -1;

    return setsockopt(sockfd, IPPROTO_IP, MCAST_LEAVE_GROUP, 
            &greq, sizeof(greq));
}

int
mcast_join_ssm(int sockfd, const char *srcaddr, 
        const char *multiaddr, const char *ifname)
{
    struct group_source_req gsreq;

    if (sockfd < 0 || srcaddr == NULL || multiaddr == NULL)
        return -1;

    if (ifname == NULL)
        gsreq.gsr_interface = 0;
    else if ((gsreq.gsr_interface = get_if_index(sockfd, ifname)) == -1)
        return -1;

    if (addr_str_to_struct(srcaddr, 0, 
                (struct sockaddr *)&gsreq.gsr_source, 
                sizeof(gsreq.gsr_source)) == -1)
        return -1;

    if (addr_str_to_struct(multiaddr, 0, 
                (struct sockaddr *)&gsreq.gsr_group, 
                sizeof(gsreq.gsr_group)) == -1)
        return -1;

    return setsockopt(sockfd, IPPROTO_IP, MCAST_JOIN_SOURCE_GROUP, 
            &gsreq, sizeof(gsreq));
}

int 
mcast_leave_ssm(int sockfd, const char *srcaddr, const char *multiaddr)
{
    struct group_source_req gsreq;

    gsreq.gsr_interface = 0;
    if (addr_str_to_struct(srcaddr, 0, 
                (struct sockaddr *)&gsreq.gsr_source, 
                sizeof(gsreq.gsr_source)) == -1)
        return -1;

    if (addr_str_to_struct(multiaddr, 0, 
                (struct sockaddr *)&gsreq.gsr_group, 
                sizeof(gsreq.gsr_group)) == -1)
        return -1;

    return setsockopt(sockfd, IPPROTO_IP, MCAST_LEAVE_SOURCE_GROUP, 
            &gsreq, sizeof(gsreq));
}
