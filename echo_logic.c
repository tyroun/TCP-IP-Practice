/*
 * =====================================================================================
 *
 *       Filename:  echo_logic.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年03月31日 23时29分43秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tyroun (), 
 *   Organization:  Verisilicon
 *
 * =====================================================================================
 */
#include "common.h"

#define MAXLINE 4096 

void base_echo_child(int sock_fd)
{
	ssize_t n;
	char buf[MAXLINE];
again:
	while((n=read(sock_fd,buf,MAXLINE))>0)	
		write(sock_fd,buf,n);
	if(n<0 && errno == EINTR)
		goto again;
	else if(n<0){
		perror("read error");
		exit(1);
	}
}

