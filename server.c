#define ERR_EXIT(m)\
	do \
	{\
		perror(m);\
		exit(EXIT_FAILURE);\
	}while(0)
	
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<string.h>
#include <sys/types.h>
#include "commsock.h"



void handle_sigchld(int sig)
{
	/*wait(); 或者 
	while(waitpid(-1,NULL,WNOHANG)>0)
	;
	依然会有僵尸进程
	*/
	int mypid=0;
	while((mypid=waitpid(-1,NULL,WNOHANG))>0)
	{
		;
		}
	}

int main(void)
{

	signal(SIGCHLD,handle_sigchld);
	signal(SIGCHLD,SIG_IGN);//忽略 避免僵尸进程
	
	int listenfd=0;
	int ret=0;
	
 	ret=sckServer_init(8001,&listenfd);
 	if(ret!=0)
 		{
 			printf("SckServer_init()err %d\n",ret);
 			return ret;
 		}	
 	while(1)
 	{
 		int ret=0;
 		int timeout=5;
 		int connfd;
 		ret=sckServer_accept(listenfd,&connfd,timeout);
 		
 			if(ret==Sck_ErrTimeOut)
 				{
 					//printf("timeout...\n");
 					continue;
 				}
 			int pid=fork();
 			if(pid==0)
 				{
 				unsigned	char recvbuf[1024];
 					int recvbuflen=1024;
 					int datalen;
 					close(listenfd);
 					while(1)
 					{
 						memset(recvbuf,0,sizeof(recvbuf));
 						ret=sckServer_rev(connfd,recvbuf,&recvbuflen,5);
 						if(ret!=0)
 							{
 								printf("func sckServer_rev()err %d\n",ret);
 								break;
 								}
 								printf("recvbuf:%s\n,recvbuf");
 								ret=sckServer_send(connfd,recvbuf,recvbuflen,6);
 								if(ret!=0)
 									{
 									printf("func sckServer_send ()err%d\n",ret);
 									break;	
 									}
 					}
 					close(connfd);
 					exit(ret);
 					
 					}
 					else if(pid>0)
 						{
 							close(connfd);
 						}
 			}
	
 			return 0;
 			
 			
}

