#include <unistd.h>
#include <stdio.h>
#include <getopt.h>

/*************************************************************************
 *    parse Option start
*************************************************************************/
char	*l_opt_arg;
char* const short_options = "htupBMUSAFT:";
struct option long_options[]={
	{"help",0,NULL,'h'},
	{"tcp",0,NULL,'t'},
	{"udp",0,NULL,'u'},
	{"port",1,NULL,'p'},
	/*the way server act*/
	{"Base",0,NULL,'B'},
	{"Multiply",0,NULL,'M'},
	{"Unblock",0,NULL,'U'},
	{"Signal",0,NULL,'S'},
	{"Aysn",0,NULL,'A'},
	{"Fork",0,NULL,'F'},
	{"Thread",0,NULL,'T'},
	{0,0,0,0},
};

void print_option(char **argv)
{
	int i,size;
	printf("%s support follow option: -%s\n",argv[0],short_options);
	size=sizeof(long_options)/sizeof(long_options[0]);
	for(i=0;i<size;i++)
		printf("-%c :  --%s\n",long_options[i].val,long_options[i].name);
}

int parse_option(int argc,char **argv)
{
	int c;
	while((c=getopt_long(argc,argv,short_options,long_options,NULL))!=1){
			switch(c){
			case 'h':
				print_option(argv);	
				break;		
			case 't':
				break;		
			case 'u':
				break;		
			case 'p':
				break;		
			case 'B':
				break;		
			case 'M':
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
				return -1;
			}
	}
	return 0;
}



/*************************************************************************
 *    parse Option end 
*************************************************************************/
