CUR_PATH=$(shell pwd)
CCFLAGS=
LIBS=-lpthread
PROGS=echo_srv #echo_cli chat_srv chat_cli proxy_srv proxy_cli 

all:	$(PROGS)
common = common.o echo_logic.o tcp_drv.o udp_drv.o

echo_srv: $(common) echo_srv.o 
	cc -g $(CCFLAGS) -o $@ $(common) echo_srv.o $(LIBS)

echo_cli: echo_cli.o
	cc -g $(CCFLAGS) -o $@ echo_cli.o $(LIBS)

chat_srv: chat_srv.o
	cc -g $(CCFLAGS) -o $@ chat_srv.o $(LIBS)
chat_cli: chat_cli.o
	cc -g $(CCFLAGS) -o $@ chat_cli.o $(LIBS)
proxy_srv: proxy_srv.o
	cc -g $(CCFLAGS) -o $@ proxy_srv.o $(LIBS)
proxy_cli: proxy_cli.o
	cc -g $(CCFLAGS) -o $@ proxy_cli.o $(LIBS)
.c.o:
	cc $(CCFLAGS) -c -g $< -o $@

clean:
	rm -f $(PROGS) *.o


