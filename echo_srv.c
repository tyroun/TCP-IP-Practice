#include "common.h"

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
	
	op->mainloop(sock_fd);

	return 0;    
}

