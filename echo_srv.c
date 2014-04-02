#include "common.h"
static int pid_cnt=0;
void sig_child(int signo)
{
	pid_t pid;
	int stat;
	while((pid = waitpid(-1,&stat,WNOHANG))>0){
		printf("child %d terminated\n",pid);
		pid_cnt++;
	}
	printf("%d child exit\n",pid_cnt);
}

void sig_int(int signo)
{
	double user,sys;
	struct rusage myusage,childusage;
	
	if(getrusage(RUSAGE_SELF,&myusage)<0)	
		perror("getrusage error");
	if(getrusage(RUSAGE_CHILDREN,&childusage)<0)	
		perror("getrusage error");

	user = (double) myusage.ru_utime.tv_sec +
		myusage.ru_utime.tv_usec/1000000.0;
	user += (double) childusage.ru_utime.tv_sec +
		childusage.ru_utime.tv_usec/1000000.0;

	sys = (double) myusage.ru_stime.tv_sec +
		myusage.ru_stime.tv_usec/1000000.0;
	sys += (double) childusage.ru_stime.tv_sec +
		childusage.ru_stime.tv_usec/1000000.0;

	printf("\nuser time = %f, sys time =%f\n",user,sys);
	printf("\nuser time = %f, sys time =%f\n",myusage.ru_utime.tv_sec,childusage.ru_utime.tv_sec);
	exit(0);
}

int main(int argc, char **argv)
{
	struct net_operation *op;
	int sock_fd;
	op=parse_option(argc,argv);
	if(!op||(!op->init)){
//		printf("Unsupport Option\n");
		exit(1);
	}
	sock_fd=op->init(op->port);

	if(op->malloc_pool){
		op->malloc_pool(10);
	}
	
	signal(SIGCHLD,sig_child);
	signal(SIGINT,sig_int);

	op->mainloop(sock_fd);

	return 0;    
}

