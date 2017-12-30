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
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/select.h>
#include "mysock.h"

typedef struct SckHandle
{
	int sockArray[100];
	int arrayNum;
	int sockfd;
	
	int contime;
	int sendtime;
	int revtime;
	
	
}SckHandle;
/*
int sckClient_init(void **handle,char *ip,int port,int contime,int sendtime,int revtime)
{
	int 	ret=0;
	
	if(handle==NULL || contime <0 || contime<0 || contime<0)
		{
			ret =Sck_ErrParam;
			printf("func sockCliet_init() err check:((handle==NULL || contime <0 || contime<0 || contime<0))%d\n",ret);
			return ret;
		}
		if(ip==NULL ||port<0 ||port>65535)
			{
				ret =Sck_ErrParam;
				printf("func sockCliet_init() err check ip==NULL ||port<0 ||port>65535 %d\n ",ret);
			}
		
		SckHandle * tmp=(SckHandle*)malloc(sizeof(SckHandle));
		
		if (tmp==NULL)
			{
				ret =Sck_ErrMalloc;
				printf("func sockCliet_init() err:malloc err%d\n",ret);
				return ret;
			}
		tmp->contime=contime;
		tmp->sendtime=sendtime;
		tmp->revtime=revtime;
		
	int sockfd;
	sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockfd<0)
		{
			ret = errno;
			printf("func sockC() err:%d\n",ret);
			return ret;
		}
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	servaddr.sin_addr.s_addr=inet_addr(ip);
	tmp->sockfd=sockfd;
	
	int connfd;
	connfd=connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	if(connfd<0)
		{
		ret =errno;
		printf("func connect() err:%d\n",ret);
		return ret;
		}
	tmp->connfd=connfd;
		return ret;
}
*/

ssize_t writen(int fd,const void* buf,size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char* bufp = (char*)buf;

    while(nleft > 0)
    {
        if((nwritten = write(fd,bufp,nleft)) < 0)
        {
            if(errno == EINTR)
                continue;
            return -1;
        }
        else if(nwritten == 0)
            continue;
        bufp += nwritten;
        nleft -= nwritten;
    }
    return count;
}

ssize_t readn(int fd,void* buf,size_t count)  
{
    size_t nleft = count;  //定义一个每次读取完剩下的字节数
    ssize_t nread;
    char* bufp = (char*)buf;

    while(nleft > 0)
    {
        if((nread = read(fd,bufp,nleft)) < 0) //read若成功返回读取的字节数目
        {
            if(errno == EINTR)         //如果是被信号打断，不算错误，重新read
                continue;
            return -1;
        }
        else if(nread == 0)
            return count-nleft;         //读到的数据为空，返回0，表示连接断开
        bufp += nread;         //读完nread字节数据之后，指针往下偏移这个量，下次的&bufd就是从这里开始
        nleft -= nread;        //这里的设定是为了多次把nleft读取完，只要nleft还有即>0，就重复这个循环    
    }   
    return count;          //全部执行正确返回读取的count，一开始是4，之后是len
}

ssize_t recv_peek(int sockfd,void *buf,size_t len)
{
	while(1)	
	{
		
		int ret=recv(sockfd,buf,len,MSG_PEEK);
		if(ret==-1 &&errno==EINTR)
			
				continue;
				return ret;
			
		
		}
	
}


int sckClient_init(void **handle,int contime,int sendtime,int revtime,int nConNum)
{
	int 	ret=0;
	
	if(handle==NULL || contime <0 || contime<0 || contime<0)
		{
			ret =Sck_ErrParam;
			printf("func sockCliet_init() err check:((handle==NULL || contime <0 || contime<0 || contime<0))%d\n",ret);
			return ret;
		}
		
		
		SckHandle * tmp=(SckHandle*)malloc(sizeof(SckHandle));
		
		if (tmp==NULL)
			{
				ret =Sck_ErrMalloc;
				printf("func sockCliet_init() err:malloc err%d\n",ret);
				return ret;
			}
		tmp->contime=contime;
		tmp->sendtime=sendtime;
		tmp->revtime=revtime;
		tmp->arrayNum=nConNum;

	/*	
	
	int sockfd;
	int i=0;
	for(i=0;i<1;i++)
	{
		
		sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		if(sockfd<0)
			{
				ret = errno;
				printf("func sockC() err:%d\n",ret);
				return ret;
			}
		tmp->sockfd=sockfd;
		
		}
		*/
	*handle=tmp;
	return ret;
}

int activate_nonblock(int fd)
{
	
	int ret=0;
	int flags=fcntl(fd,F_GETFL);
	if(flags==-1)
		{
		ret=errno;
		printf("func activate_nonblock()err :%d",ret);
		return ret;	
		}
		//ERR_EXIT("fcntl");
	flags |= O_NONBLOCK;
	ret=fcntl(fd,F_SETFL,flags);
	if(ret==-1)
		{
			printf("func activate_nonblock()err :%d",ret);
			return ret;	
		}
	return ret;
}
int deactivate_nonblock(int fd)
{
	
	int ret=0;
	int flags=fcntl(fd,F_GETFL);
	if(flags==-1)
		{
			
		
		ret=errno;
		printf("func activate_nonblock()err :%d",ret);
		return ret;	
		
			}
	flags &= ~O_NONBLOCK;
	ret=fcntl(fd,F_SETFL,flags);
	if(ret==-1)
		{
		printf("func activate_nonblock()err :%d",ret);
		return ret;	
		}
	return ret;
}




 
static int connect_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
		int ret;
		socklen_t addrlen=sizeof(struct sockaddr_in);
		if(wait_seconds>0)
				activate_nonblock(fd);
		ret=connect(fd,(struct sockaddr*)addr,addrlen);
		if(ret<0 && errno==EINPROGRESS)
			{
				fd_set connect_fdset;
				struct timeval timeout;
				FD_ZERO(&connect_fdset);
				FD_SET(fd,&connect_fdset);
				timeout.tv_sec=wait_seconds;
				timeout.tv_usec=0;
				do
				{
					ret=select(fd+1,NULL,&connect_fdset,NULL,&timeout);
					}while(ret<0 &&errno==EINTR);
				if(ret==0)
					{
						ret=-1;
						errno=ETIMEDOUT;
					}else if(ret<0)
						return -1;
					else if(ret==1)
						{
							int err;
							socklen_t socklen=sizeof(err);
							int sockoptret=getsockopt(fd,SOL_SOCKET,SO_ERROR,&err,&socklen);
							if(sockoptret==-1)
								{
									return -1;
								}
							if(err==0)
								{
									ret=0;
								}else
									{
										errno=err;
										ret=-1;
										}
							
						}
			
			}
	
	if(wait_seconds>0)
		{
			deactivate_nonblock(fd);
			}
	return ret;
}


	
	
int sckClient_getconn(void *handle,char * ip,int port,int *connfd)
{
	
	int ret=0;
	SckHandle *tmp=NULL;
	tmp=(SckHandle *)handle;
	if(handle==NULL || ip==NULL || connfd==NULL ||port<0 ||port>65536)
		{
			ret =Sck_ErrParam;
			printf("func sockCliet_init() err check:(ip || port )%d\n",ret);
			return ret;
			
		}
		int sockfd;
	sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sockfd<0)
		{
			ret = errno;
			printf("func sockC() err:%d\n",ret);
			return ret;
		}
		tmp->sockfd=sockfd;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	servaddr.sin_addr.s_addr=inet_addr(ip);
	
	
	
/*
	ret=connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	if(ret<0)
		{
		ret =errno;
		printf("func connect() err:%d\n",ret);
		return ret;
		}
		
		*/
		ret=connect_timeout(sockfd,(struct sockaddr_in *)&servaddr,(unsigned int)tmp->contime);
		if(ret<0)
			{
				if(ret==-1 && errno==ETIMEDOUT)
					{
						ret=Sck_ErrTimeOut;
						return ret;
					}else
						{
							printf("func connect_timeout()err %d\n",ret);
							}
			}
			//成功返回0 失败返回-1，超时返回-1 并errno=ETIMEDOUT
		*connfd=sockfd;
	
			return ret;
	
	
}





int write_timeout(int fd,unsigned int wait_seconds)
{
	int ret=0;
	if(wait_seconds>0)
		{
			
			  fd_set write_fdset;
				struct timeval timeout;
				FD_ZERO(&write_fdset);
				FD_SET(fd,&write_fdset);
				timeout.tv_sec=wait_seconds;
				timeout.tv_usec=0;
			do
				{
					ret=select(fd+1,NULL,&write_fdset,NULL,&timeout);
					}while(ret<0 &&errno==EINTR);
			
			if(ret==0)
				{
					ret=-1;
					errno=ETIMEDOUT;
					
					}else if(ret==1)
						ret=0;
			}
	
	return ret;
	
	
	}








int sckClient_send(void *handle,int connfd,unsigned char *data,int datalen)
{
	int ret=0;
	SckHandle *tmp=NULL;
	tmp=(SckHandle *)handle;
	ret=write_timeout(connfd,tmp->sendtime);
	if(ret==0)
		{
			int writed=0;
			unsigned char* netdata=(unsigned char*)malloc(datalen +4);
			if(netdata==NULL)
				{
					ret=Sck_ErrMalloc;
					printf("sckClient_send() malloc err:%d\n",ret);
					return ret;
					}
					int netlen=htonl(datalen);
			memcpy(netdata,&netlen,4);
			memcpy(netdata+4,data,datalen);
			writed=writen(connfd,netdata,datalen+4);
			if(writed<(datalen+4))
				{
					if(netdata!=NULL)
						{
							free(netdata);
							netdata=NULL;
							
							}
					return writed;
					
					}
		}
	if(ret<0)
		{
			if(ret==-1 && errno==ETIMEDOUT)
				{
					ret=Sck_ErrTimeOut;
				//	printf("sckClient_send() timeout err:%d\n",ret);
					return ret;
					
					}
			return ret;
		}
			return ret;
}

int read_timeout(int fd,unsigned int wait_seconds)
{
	int ret=0;
	if(wait_seconds>0)
		{
			
			  fd_set read_fdset;
				struct timeval timeout;
				FD_ZERO(&read_fdset);
				FD_SET(fd,&read_fdset);
				timeout.tv_sec=wait_seconds;
				timeout.tv_usec=0;
			do
				{
					ret=select(fd+1,&read_fdset,NULL,NULL,&timeout);
					}while(ret<0 &&errno==EINTR);
			
			if(ret==0)
				{
					ret=-1;
					errno=ETIMEDOUT;
					
					}else if(ret==1)
						ret=0;
		}
	return ret;
	
}



int sckClient_rev(void *handle,int connfd,unsigned char *out,int *outlen)
{
		int ret=0;
		if(handle==NULL||out==NULL)
			{
			 	ret=Sck_ErrParam;
			 	printf("func sckClient_rev()err %d\n",ret);
			 	return ret;
			}
		
		SckHandle *tmpHandle=(SckHandle *)handle;
		ret=read_timeout(connfd,tmpHandle->sendtime);
		if(ret!=0)
			{
				if(ret==-1 ||errno==ETIMEDOUT)
					{
						ret=Sck_ErrTimeOut;
						printf("func sckClient_rev() timeout err %d\n",ret);
						return ret;
					}else
						{	
							printf("func sckClient_rev() timeout err %d\n",ret);
							return ret;
						}
						
			}
		int netdatalen=0;
		ret=readn(connfd,&netdatalen,4);
		if(ret==-1)
			{
				printf("func readn()  err %d\n",ret);
				return ret;
				
				}else if(ret<4)
					{
						ret=Sck_ErrPeerCloser;
						printf("func readn()  err peer close %d\n",ret);
						return ret;
					}
			int n=ntohl(netdatalen);
			ret=readn(connfd,out,n);//根据长度读数据
			if(ret==-1)
			{
				printf("func readn()  err %d\n",ret);
				return ret;
				
				}else if(ret<n)
					{
						ret=Sck_ErrPeerCloser;
						printf("func readn()  err peer close %d\n",ret);
						return ret;
					}
		*outlen=n;
		return ret;
}








int sckClient_destroy(void *handle)
{
	if(handle !=NULL)
		{
			free(handle);
			}
			return 0;
}
	
	
int sckClient_closeconn(int connfd)
{
	if(connfd >=0)
		{
			close(connfd);
		}
	return 0;
	}
	
	
	/* 服务端*/
	
int sckServer_init(int port,int *listenfd)
{
	int ret;
	int mylistenfd;
	struct sockaddr_in servaddr;
	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	//servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	
	mylistenfd=socket(PF_INET,SOCK_STREAM,0);
	if(mylistenfd<0)
		{
			ret=errno;
			printf("socket()err :%d\n",ret);
			return ret;
		}
		int on=1;
	ret=setsockopt(mylistenfd, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));
	if(ret<0)	
		{	ret=errno;
			printf("setsockopt()err :%d\n",ret);
			return ret;
		}
	ret=bind(mylistenfd, (struct sockaddr *)&servaddr,sizeof(servaddr));
		if(ret<0)
		{	
			ret=errno;
			printf("bind()err :%d\n",ret);
			return ret;
			
		}
	
	
ret=listen(mylistenfd,SOMAXCONN);//使用listen将会阻塞
	if(ret<0)
	{	
			ret=errno;
			printf("listen()err :%d\n",ret);
			return ret;
	}
	
	
	*listenfd=mylistenfd;
	return 0;
}


int accept_timeout(int fd,struct sockaddr_in *addr,unsigned int wait_seconds)
{
	int ret;
	socklen_t addrlen=sizeof(struct sockaddr_in);
	if(wait_seconds>0)
		{
			
			 fd_set accept_fdset;
				struct timeval timeout;
				FD_ZERO(&accept_fdset);
				FD_SET(fd,&accept_fdset);
				timeout.tv_sec=wait_seconds;
				timeout.tv_usec=0;
			do
				{
					ret=select(fd+1,&accept_fdset,NULL,NULL,&timeout);
					}while(ret<0 && errno==EINTR);
			
			if(ret==-1)
				
					return -1;
				else if(ret==0)
						{
							errno=ETIMEDOUT;
							return -1;
						}
		}

			
			
		if(addr!=NULL)
			
				ret=accept(fd,(struct sockaddr*)addr,&addrlen);
		else
			ret=accept(fd,NULL,NULL);
		if(ret==-1)
				{
				ret=errno;
				printf("func accept()err :%d \n ",ret);
				return ret;
					
				}
			
	return ret;
	}




int sckServer_accept(int listenfd,int *connfd,int timeout)
{	
	int ret=0;
	ret=accept_timeout(listenfd,NULL,(unsigned int)timeout);
	if(ret<0)
		{	
			if(ret==-1 && errno==ETIMEDOUT)	
			{
				ret=Sck_ErrTimeOut;
				printf("accept_timeout() err %d\n",ret);
				return ret;
				}
				else
					{
						printf("func sckServer_accept()err :%d\n",ret);
						return ret;
					}
		
			}
		*connfd=ret;
		return 0;
}

//服务端发送报文
int sckServer_send(int connfd,unsigned char *data,int datalen,int timeout)
{
	int ret=0;
	ret=write_timeout(connfd,timeout);
	if(ret==0)
		{
			
			int writed=0;
			unsigned char* netdata=(unsigned char*)malloc(datalen +4);
			if(netdata==NULL)
				{
					ret=Sck_ErrMalloc;
					printf("sckServer_send() malloc err:%d\n",ret);
					return ret;
					}
			int netlen=htonl(datalen);
			memcpy(netdata,&netlen,4);
			memcpy(netdata+4,data,datalen);
			writed=writen(connfd,netdata,datalen+4);
			if(writed<(datalen+4))
				{
					if(netdata!=NULL)
						{
							free(netdata);
							netdata=NULL;
							
							}
					return writed;
					
					}
		}
	if(ret<0)
		{
			if(ret==-1 && errno==ETIMEDOUT)
				{
					ret=Sck_ErrTimeOut;
					printf("sckServer_send() timeout err:%d\n",ret);
					return ret;
					
					}
			return ret;
		}
			return ret;

}
//服务端接收报文
int sckServer_rev(int connfd,unsigned char *out,int *outlen,int timeout)
{
	int ret=0;
		if(out==NULL || outlen==NULL)
			{
			 	ret=Sck_ErrParam;
			 	printf("func sckServer_rev()err %d\n",ret);
			 	return ret;
			}
		
		
		ret=read_timeout(connfd,timeout);
		if(ret!=0)
			{
				if(ret==-1 && errno==ETIMEDOUT)
					{
						ret=Sck_ErrTimeOut;
						printf("func sckServer_rev() timeout err %d\n",ret);
						return ret;
					}else
						{	
							printf("func sckServer_rev() timeout err %d\n",ret);
							return ret;
						}
						
			}
		int netdatalen=0;
		ret=readn(connfd,&netdatalen,4);
		if(ret==-1)
			{
				printf("func readn()  err %d\n",ret);
				return ret;
				
				}
				else if(ret < 4)
					{
						ret=Sck_ErrPeerCloser;
						printf("func readn()  err peer close %d\n",ret);
						return ret;
					}
			int n;
			n=ntohl(netdatalen);
			ret=readn(connfd,out,n);//根据长度读数据
			if(ret==-1)
			{
				printf("func readn()  err %d\n",ret);
				return ret;
				
				}
				else if(ret<n)
					{
						ret=Sck_ErrPeerCloser;
						printf("func readn()  err peer close %d\n",ret);
						return ret;
					}
		*outlen=n;
		return 0;
}
//服务端释放
int sckServer_destroy(int *listenfd)
{
	
	return 0;
	
}
	
