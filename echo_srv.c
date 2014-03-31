#include <unistd.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

int main(int argc, char **argv)
{
	int sock_fd, listen_fd;
	struct sockaddr srv_addr;

    sock_fd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);	

	srv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	if(bind(sock_fd,srv_addr,sizeof(srv_addr))<0){
			perror("bind  error");
			exit(1);
	}

	listen_fd=listen(sock_fd,10);
	if(listen_fd<0){
			perror("listen error");
			exit(1);
	}
	

	return 0;    
}

