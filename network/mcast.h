/*
 * mcast.h add/leave an multicast group (or SSM group).
 */
#ifndef __MCAST_H__
#define __MCAST_H__

/*
 * Function:
 *   Join an any-source multicast group.
 * Parameters:
 *   $sockfd [IN], on which socket to join.
 *   $multiaddr [IN], to join which multicast group.
 *   $ifname [IN], on which interface to join or NULL for default.
 * Return:
 *   0 if success and -1 if error happened.
 */
int
mcast_join(int sockfd, const char *multiaddr, const char *ifname);

/*
 * Function:
 *   Leave an any-souce multicast group.
 * Parameters:
 *   $sockfd [IN], on which socket to leave.
 *   $multiaddr [IN], to leave which multicast group.
 * Return:
 *   0 if success and -1 if error happened.
 */
int
mcast_leave(int sockfd, const char *multiaddr);

/*
 * Function:
 *   Join an source specified multicast (SSM) group.
 * Parameters:
 *   $sockfd [IN], on which socket to join.
 *   $srcaddr [IN], source address of group to join.
 *   $multiaddr [IN], multicast address of group to join.
 *   $ifname [IN], on which interface to join or NULL for default.
 * Return:
 *   0 if success and -1 if error happened.
 */
int
mcast_join_ssm(int sockfd, const char *srcaddr, const char *multiaddr, const char *ifname);

/*
 * Function:
 *   leave an source specified multicast (SSM) group.
 * Parameters:
 *   $sockfd [IN], on which socket to leave.
 *   $srcaddr [IN], source address of group to leave.
 *   $multiaddr [IN], multicast address of group to leave.
 * Return:
 *   0 if success and -1 if error happened.
 */
int 
mcast_leave_ssm(int sockfd, const char *srcaddr, const char *multiaddr);

#endif /* __MCAST_H__ */
