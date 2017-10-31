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
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
//定义一个带len的数据包结构
struct packet
{
    int len;            //记录
    char buf[1024];     //这里的1024只做单个最长数据包最长的限定
};

//简单说下这里的ssize_t：是有符号的整形，在32位机上是4个字节的long，在64位机上是8个字节的long，这个区别暂时不多讲，之后作者会单独写一个大致的总结聊聊32和64位的由来相关和异同
//size_t是无符号的整形
//封装一个readn
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
//还是简要讲一下这个函数的功能，它是照着write的原型来的，从传入的buf这个指针所指的位置开始，发送大小为count的数据，read也是一样，同理可以知道它的作用，不同的是我们在read之上加上了返回值，实现我们需要的功能
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


/*接下来我们将使用readn和writen应答的过程封装成一个函数，在fork子进程的时候调用—-
就是之前我们写过的先读取read缓冲区，然后回射write到客户端。
不同的是这里我们都是用自己封装的readn和writen，还加上了二次读取的判断。 */


/*
最后是main中的代码—-先理清思路，前面还是一样，创建一个套接字，
bind地址，监听它，之后等待accept事件发生，一旦客户端连接上来，
fork一个子进程来处理读取操作，父进程继续监听是否有新的客户端连接，
有的话继续fork….  */

//创建，bind，监听...等等代码就省略了，详情看前面几章，接下来直接写对连入的客户端的操作

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

ssize_t readline(int sockfd,void *buf,size_t maxline)

{
		int ret=0;
		int nread;
		char *bufp=buf;
		int nleft=maxline;
		while(1)
		{
			ret=recv_peek(sockfd,bufp,nleft);
			if(ret<0)
				return ret;
			else if(ret==0)
				return ret;
				
			nread=ret;
			int i;
			for(i=0;i<nread;i++)	
			{
				if(bufp[i]=='\n')
					{
						ret=readn(sockfd,bufp,i+1);
						if(ret!=i+1)
							exit(EXIT_FAILURE);
						return ret;
					}
			}
				
				if(nread>nleft)
					exit(EXIT_FAILURE);
					
				nleft-=nread;
				ret=readn(sockfd,bufp,nread);
				if(ret != nread)
					exit(EXIT_FAILURE);
				bufp+=nread;
		}

return -1;




}

void echo_srv(int conn)
{
	
	char recvbuf[1024];
	while(1)
	{
		memset(recvbuf,0,sizeof(recvbuf));
		int ret=readline(conn,recvbuf,1024);
		 if(ret == -1)
            ERR_EXIT("readnline");
     if(ret == 0)
           {
            printf("client close"); 
            break;     
					}
					
					fputs(recvbuf,stdout);
					writen(conn,recvbuf,strlen(recvbuf));
					if(recvbuf[0]=='2')
						{
							//close(conn);
							//close(conn);
							shutdown(conn,SHUT_WR);
							}
	}
	
}

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
	//signal(SIGCHLD,SIG_IGN);//忽略 避免僵尸进程
	signal(SIGCHLD,handle_sigchld);
	struct sockaddr_in servaddr;
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(8001);
	servaddr.sin_addr.s_addr=inet_addr("127.0.0.1");
	//servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	int listenfd;
	listenfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(listenfd==-1)
		{
			perror("func socket\n erro");
			exit(0);
		}
		int on=1;
	if(setsockopt(listenfd, SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on))<0)
		{
			perror("setsockopt err\n ");
			exit(0);
		}
	if(bind(listenfd, (struct sockaddr *)&servaddr,sizeof(servaddr))<0)
		{
		perror("bind err\n");
		exit(0);	
			
		}
	//一旦调用了listen函数，这个套接字sockfd将变成被动套接字，被动套接字只能接收连接，不能发送连接
	//listen 做了两个队列。。。已完成连接队列和未完成连接队列
if(listen(listenfd,SOMAXCONN)==-1)
	{
		perror("listen err\n");
		exit(0);	
	}
   

    struct sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);

    int conn;
    pid_t pid;

    while(1)
    {
        if((conn = accept(listenfd,(struct sockaddr*)&peeraddr,&peerlen)) < 0)
            ERR_EXIT("accept");
        printf("ip地址是：%s 端口是：%d\n",inet_ntoa(peeraddr.sin_addr),ntohs(peeraddr.sin_port));

        pid = fork();
        if(pid == -1)
            ERR_EXIT("fork");
        if(pid == 0)
        {
            close(listenfd);
            echo_srv(conn);
            exit(EXIT_SUCCESS);        
        }
        else
        { //  close(conn);
        	}
    }
    return 0;
}

