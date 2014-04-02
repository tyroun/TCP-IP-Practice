/*
 * =====================================================================================
 *
 *       Filename:  common.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年03月31日 17时29分55秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tyroun (), 
 *   Organization:  Verisilicon
 *
 * =====================================================================================
 */
#ifndef __common_H__
#define __common_H__
#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/resource.h>

struct net_operation{
	int (*init)(short port);
	int (*malloc_pool)(int pool_size);
	void (*mainloop)(int sockfd);
	int port;
}; 

struct net_operation *parse_option(int argc,char **argv);

void tcp_base_loop(int sock_fd);
int tcp_init(short port);
void tcp_select_loop(int sock_fd);


void udp_base_loop(int sock_fd);
int udp_init(short port);

#endif

