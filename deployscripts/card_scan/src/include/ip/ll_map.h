/*
*   ip.c     "ip" utility frontend.
*   This program is free software; you can redistribute it and/or
*   modify it under the terms of the GNU General Public License
*   as published by the Free Software Foundation; either version
*   2 of the License, or (at your option) any later version.
*   Authors: Alexey Kuznetsov, <kuznet@ms2.inr.ac.ru>
*/

#ifndef __LL_MAP_H__
#define __LL_MAP_H__ 1

extern int ll_remember_index(const struct sockaddr_nl *who,
			     struct nlmsghdr *n, void *arg);
extern int ll_init_map(struct rtnl_handle *rth);
extern unsigned ll_name_to_index(const char *name);
extern const char *ll_index_to_name(unsigned idx);
extern const char *ll_idx_n2a(unsigned idx, char *buf);
extern int ll_index_to_type(unsigned idx);
extern unsigned ll_index_to_flags(unsigned idx);
extern unsigned ll_index_to_addr(unsigned idx, unsigned char *addr,
				 unsigned alen);

#endif /* __LL_MAP_H__ */
