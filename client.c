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


int main()
{
	int 	ret=0;
	void 	*handle =NULL;
	
	
	int connfd;
	unsigned char *data=(unsigned char*)"aaaaaaffffffffff";
	int 	datalen=10;
	unsigned char out[1024];
	int outlen=1024;
	ret=sckClient_init(&handle,15,5,5,10);
	ret=sckClient_getconn(handle,"127.0.0.1",8001,&connfd);

	ret=sckClient_send(handle,connfd,data,datalen);
	out[outlen]='\0';
	printf("data: %s \n",data);
	ret=sckClient_rev(handle,connfd,out,&outlen);
printf("11111");
	ret=sckClient_destroy(handle);
	return 0;
}