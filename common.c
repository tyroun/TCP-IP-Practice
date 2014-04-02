#include <getopt.h>
#include "common.h"

/*************************************************************************
 *    parse Option start
*************************************************************************/
static char	*l_opt_arg;
static char* const short_options = "p:htuBMEUSAFT";
static struct net_operation op={
	0,0,0,0
};
static struct option long_options[]={
	{"help",0,NULL,'h'},
	{"tcp",0,NULL,'t'},
	{"udp",0,NULL,'u'},
	{"port",1,NULL,'p'},
	/*the way server act*/
	{"Base",0,NULL,'B'},
	{"Multiply",0,NULL,'M'},
	{"Epoll",0,NULL,'E'},
	{"Unblock",0,NULL,'U'},
	{"Signal",0,NULL,'S'},
	{"Ansy",0,NULL,'A'},
	{"Fork",0,NULL,'F'},
	{"Thread",0,NULL,'T'},
	{0,0,0,0},
};


static void print_option(char **argv)
{
	int i,size;
	printf("%s support follow option: -%s\n",argv[0],short_options);
	size=sizeof(long_options)/sizeof(long_options[0]);
	for(i=0;i<size-1;i++)
		printf("-%c :  --%s\n",long_options[i].val,long_options[i].name);
}

struct net_operation *parse_option(int argc,char **argv)
{
	int c;
	int isTcpFlag=0;
	while((c=getopt_long(argc,argv,short_options,long_options,NULL))!=-1){
			switch(c){
			case 'h':
				print_option(argv);
				return 0;	
			case 't':
				op.init=tcp_init;
				isTcpFlag=1;
				break;		
			case 'u':
				op.init=udp_init;
				isTcpFlag=0;
				break;		
			case 'p':
				if(!optarg){
					printf("Please input port number\n");
					return 0;
				}
				l_opt_arg=optarg;
				op.port=atoi(l_opt_arg);
				printf("port=%d\n",op.port);
				break;		
			case 'B':
				if(isTcpFlag)
					op.mainloop=tcp_base_loop;				
				else
					op.mainloop=udp_base_loop;
				break;		
			case 'M':
				op.mainloop=tcp_select_loop;
				break;	
			case 'E':
				break;
			case 'U':
				break;		
			case 'S':
				break;		
			case 'A':
				break;		
			case 'F':
				break;		
			case 'T':
				break;		
			defaut:
				printf("unknown option\n");
				print_option(argv);
				return 0;
			}
	}
	return &op;
}



/*************************************************************************
 *    parse Option end 
*************************************************************************/
