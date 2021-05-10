/*
*   ip.c     "ip" utility frontend.
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   as published by the Free Software Foundation; either version
*   2 of the License, or (at your option) any later version.
*   Authors: Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include "ip/utils.h"
#include "ip/ip_common.h"

#define IFF_UP_TMP 0x1     /* interface is up      */
#define	IFNAMSIZ_TMP 16
#define IDXMAP_SIZE 1024

struct ll_cache
{
    struct ll_cache *idx_next;
    unsigned flags;
    int	index;
    unsigned short type;
    unsigned short alen;
    char name[IFNAMSIZ_TMP];
    unsigned char addr[20];
};

struct iplink_req {
    struct nlmsghdr n;
    struct ifinfomsg i;
    char buf[1024];
};

struct rtnl_handle rth = { .fd = -1 };
static struct ll_cache *idx_head[IDXMAP_SIZE];
static int iplink_modify(int, unsigned int, int , char **);
int rtnl_open(struct rtnl_handle *, unsigned);
int do_iplink(int , char **);
void rtnl_close(struct rtnl_handle *);
int rtnl_open_byproto(struct rtnl_handle *, unsigned ,int);
int matches(const char *, const char *);
unsigned ll_name_to_index(const char *);
int rtnl_talk(struct rtnl_handle *, struct nlmsghdr *, pid_t, unsigned , struct nlmsghdr *);
void incomplete_command();
int iplink_parse(int , char **, struct iplink_req *, char **, char **, char **, char **, int *);
int addattr_l(struct nlmsghdr *, int, int, const void *, int);

int preferred_family = AF_UNSPEC;
int rcvbuf = 1024 * 1024;

void ip(char *net_up, char *net_down, int status) {
	char *up[4];
	char *down[4];
	char *rename[5];

	up[2] = (char *)malloc(sizeof(char) * 10);
	down[2] = (char *)malloc(sizeof(char) * 10);
	rename[2] = (char *)malloc(sizeof(char) * 10); // old ethx
	rename[4] = (char *)malloc(sizeof(char) * 10); // new ethx

	if (up[2] == NULL || down[2] == NULL || rename[2] == NULL || rename[4] == NULL) {
		printf("Not enough memory\n");
		exit(-1);
	}

	rtnl_open(&rth, 0);
	
	up[0] = "set";
	up[1] = "dev";
	up[3] = "up";
	
	down[0] = "set";
	down[1] = "dev";
	down[3] = "down";

	rename[0] = "set";
	rename[1] = "dev";
	rename[3] = "name";
	
	switch (status) {
	case 1: // up
		strcpy(up[2], net_up);
		do_iplink(4, up);
		break;
	case 2: // down
		strcpy(down[2], net_down);
		do_iplink(4, down);
		break;
	case 3: // rename
		strcpy(rename[2], net_up);
		strcpy(rename[4], net_down);
		do_iplink(5, rename);
		break;
	}
	rtnl_close(&rth);

	free(up[2]);
	free(down[2]);
	free(rename[2]);
	free(rename[4]);
}

int rtnl_open(struct rtnl_handle *rth, unsigned subscriptions)
{
    return rtnl_open_byproto(rth, subscriptions, NETLINK_ROUTE);
}

int do_iplink(int argc, char **argv)
{
    return iplink_modify(RTM_NEWLINK, 0, argc-1, argv+1);
}

void rtnl_close(struct rtnl_handle *rth)
{
    if (rth->fd >= 0) {
        close(rth->fd);
        rth->fd = -1;
    }
}


int rtnl_open_byproto(struct rtnl_handle *rth, unsigned subscriptions,
              int protocol)
{
    socklen_t addr_len;
    int sndbuf = 32768;

    memset(rth, 0, sizeof(*rth));

    rth->fd = socket(AF_NETLINK, SOCK_RAW, protocol);
	setsockopt(rth->fd,SOL_SOCKET,SO_SNDBUF,&sndbuf,sizeof(sndbuf));
	setsockopt(rth->fd,SOL_SOCKET,SO_RCVBUF,&rcvbuf,sizeof(rcvbuf));

    memset(&rth->local, 0, sizeof(rth->local));
    rth->local.nl_family = AF_NETLINK;
    rth->local.nl_groups = subscriptions;

	bind(rth->fd, (struct sockaddr*)&rth->local, sizeof(rth->local));
    
	addr_len = sizeof(rth->local);
	getsockname(rth->fd, (struct sockaddr*)&rth->local, &addr_len);
    
	rth->seq = time(NULL);
    return 0;
}

int matches(const char *cmd, const char *pattern)
{
    int len = strlen(cmd);
    if (len > strlen(pattern))
        return -1;
    return memcmp(pattern, cmd, len);
}

static int iplink_modify(int cmd, unsigned int flags, int argc, char **argv)
{
    int len;
    char *dev = NULL;
    char *name = NULL;
    char *link = NULL;
    char *type = NULL;
    int group;
    struct iplink_req req;
    int ret;

    memset(&req, 0, sizeof(req));

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
    req.n.nlmsg_flags = NLM_F_REQUEST|flags;
    req.n.nlmsg_type = cmd;
    req.i.ifi_family = preferred_family;

    ret = iplink_parse(argc, argv, &req, &name, &type, &link, &dev, &group);
    if (ret < 0)
        return ret;

    argc -= ret;
    argv += ret;


    req.i.ifi_index = ll_name_to_index(dev);

    if (name) {
        len = strlen(name) + 1;
        addattr_l(&req.n, sizeof(req), IFLA_IFNAME, name, len);
    }

	rtnl_talk(&rth, &req.n, 0, 0, NULL);

    return 0;
}

int iplink_parse(int argc, char **argv, struct iplink_req *req, char **name, char **type, char **link, char **dev, int *group)
{
    int ret;

    *group = -1;
    ret = argc;

    while (argc > 0) {
        if (strcmp(*argv, "up") == 0) {
            req->i.ifi_change |= IFF_UP_TMP;
            req->i.ifi_flags |= IFF_UP_TMP;
        } else if (strcmp(*argv, "down") == 0) {
            req->i.ifi_change |= IFF_UP_TMP;
            req->i.ifi_flags &= ~IFF_UP_TMP;
        } else if (strcmp(*argv, "name") == 0) {
            NEXT_ARG();
            *name = *argv;
        } else if (matches(*argv, "link") == 0) {
            NEXT_ARG();
            *link = *argv;
        }
        else {
            if (strcmp(*argv, "dev") == 0) {
                NEXT_ARG();
            }
            *dev = *argv;
        }
        argc--; argv++;
    }

    return ret - argc;
}                        

unsigned ll_name_to_index(const char *name)
{
    static char ncache[IFNAMSIZ_TMP];
    static int icache;
    struct ll_cache *im;
    int i;
    unsigned idx;

    for (i=0; i<IDXMAP_SIZE; i++) {
        for (im = idx_head[i]; im; im = im->idx_next) {
            if (strcmp(im->name, name) == 0) {
                icache = im->index;
                strcpy(ncache, name);
                return im->index;
            } else
		icache = icache;
        }
    }

    idx = if_nametoindex(name);
    if (idx == 0)
        sscanf(name, "if%u", &idx);
    return idx;
}

int addattr_l(struct nlmsghdr *n, int maxlen, int type, const void *data, int alen)
{
    int len = RTA_LENGTH(alen);
    struct rtattr *rta;

    rta = NLMSG_TAIL(n);
    rta->rta_type = type;
    rta->rta_len = len;
    memcpy(RTA_DATA(rta), data, alen);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
    return 0;
}

int rtnl_talk(struct rtnl_handle *rtnl, struct nlmsghdr *n, pid_t peer, unsigned groups, struct nlmsghdr *answer)
{
    int status;
    unsigned seq;
    struct nlmsghdr *h;
    struct sockaddr_nl nladdr;
    
	struct iovec iov = {
        .iov_base = (void*) n,
        .iov_len = n->nlmsg_len
    };
    
	struct msghdr msg = {
        .msg_name = &nladdr,
        .msg_namelen = sizeof(nladdr),
        .msg_iov = &iov,
        .msg_iovlen = 1,
    };
    char   buf[16384];

    memset(&nladdr, 0, sizeof(nladdr));
    nladdr.nl_family = AF_NETLINK;
    nladdr.nl_pid = peer;
    nladdr.nl_groups = groups;

    n->nlmsg_seq = seq = ++rtnl->seq;

    if (answer == NULL)
        n->nlmsg_flags |= NLM_F_ACK;

    status = sendmsg(rtnl->fd, &msg, 0);

    memset(buf,0,sizeof(buf));

    iov.iov_base = buf;

    while (1) {
        iov.iov_len = sizeof(buf);
        status = recvmsg(rtnl->fd, &msg, 0);

        for (h = (struct nlmsghdr*)buf; status >= sizeof(*h); ) {
            int len = h->nlmsg_len;
            int l = len - sizeof(*h);

            if (nladdr.nl_pid != peer ||
                h->nlmsg_pid != rtnl->local.nl_pid ||
                h->nlmsg_seq != seq) {
                /* Don't forget to skip that message. */
                status -= NLMSG_ALIGN(len);
                h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
                continue;
            }

            if (h->nlmsg_type == NLMSG_ERROR) {
				struct nlmsgerr *err = (struct nlmsgerr*)NLMSG_DATA(h);
                if (l < sizeof(struct nlmsgerr)) {
                    fprintf(stderr, "ERROR truncated\n");
                } else {
                    if (!err->error) {
						if (answer)
                            memcpy(answer, h, h->nlmsg_len);
                        return 0;
                                                                                                       
                    }

                    fprintf(stderr, "RTNETLINK answers: %s\n", strerror(-err->error));
                    errno = -err->error;
                }
                return -1;
            }
            
			if (answer) {
                memcpy(answer, h, h->nlmsg_len);
                return 0;
            }

            status -= NLMSG_ALIGN(len);
            h = (struct nlmsghdr*)((char*)h + NLMSG_ALIGN(len));
        }
    }
}

void incomplete_command(void)
{
    fprintf(stderr, "Command line is not complete. Try option \"help\"\n");
    exit(-1);
}

