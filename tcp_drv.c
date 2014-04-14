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
#include <sys/epoll.h>
#include <fcntl.h>

#define SA  struct sockaddr

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
	listen_fd=listen(sock_fd,500);
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

	fcntl(sock_fd,F_SETFL,fcntl(sock_fd,F_GETFL)|O_NONBLOCK);/*important!! make sure accept after select is noblock */

	listen_fd=listen(sock_fd,500);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	printf("now start to wait for client\n");

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
			if(conn_fd<0){
				//ignore following error
				if((errno==EWOULDBLOCK)||/*berkeley:client disconnect*/\
				 (errno==ECONNABORTED)||/*posix: client disconnect*/\
		 		 (errno==EPROTO)||/*svr4: client disconnect*/\
				 (errno==EINTR) )
					printf("ignore");
				else{
					perror("accept error");
					exit(1);
				}
			}
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
				/*this read/write way has 2 issue:
				 *1:	program is block in read/write, so it is still not noblock
				 *2:	client attack, client will send a char and hold whole server
				 * */
					close(client_fd[i]);
					FD_CLR(client_fd[i],&allset);
					client_fd[i]=-1;
				}


			}
		}
	}
}

/************************************************************************************************
 *  absolute noblock version; include noblock read and write
************************************************************************************************/
#define MAX_EVENTS 1024
#define MAXLINE 4096
struct event_data{
	char buf[MAXLINE];
	int rptr;
	int wptr;
	int sock_fd;
	int isClose;
};


void tcp_epoll_loop_noblock(int sock_fd)
{
	int listen_fd,conn_fd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	/*val used for epoll*/	
	struct epoll_event ev,con_ev,events[20];
	struct event_data data[20];

	int epfd,nfds;
	int i,j;

	fcntl(sock_fd,F_SETFL,fcntl(sock_fd,F_GETFL)|O_NONBLOCK);/*Important!! make sure accept after select is noblock */
	listen_fd=listen(sock_fd,1024);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	printf("Now start to wait for client\n");

	epfd= epoll_create(MAX_EVENTS);
	ev.data.fd=sock_fd;
	ev.events=EPOLLIN;

	//register epoll event
	epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);

	clilen=sizeof(cli_addr);

	for(j=0;j<20;j++){
		data[j].rptr=0;
		data[j].wptr=0;
		bzero(data[j].buf,MAXLINE);
		data[j].sock_fd=-1;
		data[j].isClose=0;
	}

	for(;;){
		//wait epoll events happen
		nfds=epoll_wait(epfd,events,20,500);	
		//process all events
		for(i=0;i<nfds;++i){
			if(events[i].data.fd==sock_fd){/*accept a connect*/
				conn_fd=accept(sock_fd,(SA *)&cli_addr,&clilen);	
				if(conn_fd<0){
					perror("accept error");
					exit(1);
				}
				for(j=0;j<21;j++){
					if(j==20){
						printf("too many connection\n");
						exit(1);
					}
					if(data[j].sock_fd==-1)
						break;
				}
				con_ev.data.ptr=(void*)&data[j];
				data[j].sock_fd=conn_fd;
				con_ev.events=EPOLLIN|EPOLLOUT;
				epoll_ctl(epfd,EPOLL_CTL_ADD,conn_fd,&con_ev);
				fcntl(conn_fd,F_SETFL,fcntl(conn_fd,F_GETFL)|O_NONBLOCK);
			}
			else if(events[i].events&EPOLLIN){/*get data from client*/
				struct event_data *d=(struct event_data *)events[i].data.ptr;
				int nbytes;
				if((listen_fd=d->sock_fd)<0)
					continue;
				nbytes=read(listen_fd,&d->buf[d->rptr],MAXLINE-d->rptr);
				if(nbytes==0){
					/*client close*/
					d->isClose=1;
					events[i].events=EPOLLOUT;
					epoll_ctl(epfd,EPOLL_CTL_MOD,listen_fd,&events[i]);
				}
				else if(nbytes<0){
					perror("read error");
					exit(1);
				}
				else if(nbytes==MAXLINE-d->rptr){
					d->rptr=0;
				}
				else{
					d->rptr+=nbytes;
				}
			}
			else if(events[i].events&EPOLLOUT){	/*send data to client*/
				struct event_data *d=(struct event_data *)events[i].data.ptr;
				int nbytes,wbytes;
				if((listen_fd=d->sock_fd)<0)
					continue;
				wbytes=(d->wptr<d->rptr)?d->rptr-d->wptr:MAXLINE-d->wptr;
				nbytes=write(listen_fd,&d->buf[d->wptr],wbytes);	
				if(nbytes<=0){ /*write should not return 0, because we control close server connection*/
					perror("write error");
					exit(1);
				}
				d->wptr+=nbytes;
				if(d->wptr==MAXLINE)
					d->wptr=0;
				if(d->wptr==d->rptr&&(d->isClose==1)){  /*When do read, we already find client close connection; Now is our turn*/
					close(listen_fd);
					epoll_ctl(epfd,EPOLL_CTL_DEL,listen_fd,&events[i]);
					d->rptr=0;
					d->wptr=0;
					d->isClose=0;
					d->sock_fd=-1;
				}

			}				
		}
	}
}


/****************************************************************************************************
 * 	epoll way is quit differnt, just start below
****************************************************************************************************/

void tcp_epoll_loop(int sock_fd)
{
	int listen_fd,conn_fd;
	struct sockaddr_in cli_addr;
	socklen_t clilen;
	/*val used for epoll*/	
	struct epoll_event ev,events[20];

	int epfd,nfds;
	int i;

	fcntl(sock_fd,F_SETFL,fcntl(sock_fd,F_GETFL)|O_NONBLOCK);/*Important!! make sure accept after select is noblock */
	listen_fd=listen(sock_fd,500);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	printf("Now start to wait for client\n");

	epfd= epoll_create(MAX_EVENTS);
	ev.data.fd=sock_fd;
	ev.events=EPOLLIN;

	//register epoll event
	epoll_ctl(epfd,EPOLL_CTL_ADD,sock_fd,&ev);

	clilen=sizeof(cli_addr);
	for(;;){
		//wait epoll events happen
		nfds=epoll_wait(epfd,events,20,500);	
		//process all events
		for(i=0;i<nfds;++i){
			if(events[i].data.fd==sock_fd){/*accept a connect*/
				conn_fd=accept(sock_fd,(SA *)&cli_addr,&clilen);	
				if(conn_fd<0){
					perror("accept error");
					exit(1);
				}
				ev.data.fd=conn_fd;
				ev.events=EPOLLIN;
				epoll_ctl(epfd,EPOLL_CTL_ADD,conn_fd,&ev);
			}
			else if(events[i].events&EPOLLIN){/*get data from client*/
				if((listen_fd=events[i].data.fd)<0)
					continue;
				if(!echo_single_slect(listen_fd)){	
				/*this read/write way has 2 issue:
				 *1:	program is block in read/write, so it is still not noblock
				 *2:	client attack, client will send a char and hold whole server
				 * */
					close(listen_fd);
					events[i].data.fd=-1;
					ev.data.fd=listen_fd;
					ev.events=EPOLLIN;
					epoll_ctl(epfd,EPOLL_CTL_DEL,listen_fd,&ev);
				}
			}
		}
	}
}

/****************************************************************************************************
 * 	epoll way is quit differnt, end now 
****************************************************************************************************/

