/*
 * =====================================================================================
 *
 *       Filename:  tcp_drv.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年03月31日 18时01分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Tyroun (), 
 *   Organization:  Verisilicon
 *
 * =====================================================================================
 */
#include "common.h"
#include <sys/select.h>
#include <sys/time.h>

#define SA const struct sockaddr

int tcp_init(short port)
{
	int sock_fd;
	struct sockaddr_in srv_addr;

 	sock_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock_fd<0){
		perror("sock error");
		exit(1);
	}
	bzero(&srv_addr,sizeof(srv_addr));
	srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	srv_addr.sin_family=AF_INET;
	srv_addr.sin_port=htons(port);
	if(bind(sock_fd,(SA *)&srv_addr,sizeof(srv_addr))<0){
			perror("bind  error");
			exit(1);
	}

	return sock_fd;	
}

void tcp_base_loop(int sock_fd)
{
	int listen_fd,conn_fd;
	pid_t child_pid;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	listen_fd=listen(sock_fd,100);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	printf("Now start to wait for client\n");
	for(;;){
		clilen=sizeof(cli_addr);
		conn_fd=accept(sock_fd,(SA *)&cli_addr,&clilen);
		if(conn_fd<0){
			if(errno==EINTR)
				continue;
			else{
				perror("accept error");
				exit(1);
			}
		}
		if ( (child_pid=fork())==0  ) {
			// in child
			close(sock_fd);
			base_echo_child(conn_fd);
			exit(0);
		}
		close(conn_fd);
	}
}

void tcp_select_loop(int sock_fd)
{
	int listen_fd,conn_fd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	/*val used for select*/	
	fd_set rset,allset;
	int maxfd,nready,maxi;
	int i;
	int client_fd[FD_SETSIZE];

	listen_fd=listen(sock_fd,10);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	printf("Now start to wait for client\n");

	FD_ZERO(&allset);	

	FD_SET(sock_fd,&allset);
	maxfd=sock_fd;

	for(i=0;i<FD_SETSIZE;i++)
		client_fd[i]=-1;
	maxi=-1;

	for(;;){
		rset=allset;
		nready=select(maxfd+1,&rset,NULL,NULL,NULL);
		if(FD_ISSET(sock_fd,&rset)){
			/*accept new connect*/
			clilen=sizeof(cli_addr);
			conn_fd=accept(sock_fd,(SA *)&cli_addr,&clilen);
			for(i=0;i<FD_SETSIZE;++i){
				if(client_fd[i]<0){
					client_fd[i]=conn_fd;
					break;
				}	
			}
			FD_SET(conn_fd,&allset);
			if(conn_fd>maxfd)
				maxfd=conn_fd;
			if(i+1>maxi)
				maxi=i+1;
			if(--nready<=0)
				continue;
		}
		/*now check if accept new data from client*/
		for(i=0;i<maxi;i++){
			if(client_fd[i]<0)		
				continue;
			if(FD_ISSET(client_fd[i],&rset)){
			/*now get data from client*/
				if(!echo_single_slect(client_fd[i])){	
					close(client_fd[i]);
					FD_CLR(client_fd[i],&allset);
					client_fd[i]=-1;
				}


			}
		}
	}
}

