#ifndef SOCKETLIB_H_
#define SOCKETLIB_H_



#ifdef __cplusplus
extern 'c'
{
#endif
//错误码
#define Sck_Ok  							0
#define Sck_BaseErr 					3000
#define Sck_ErrParam  				(Sck_BaseErr+1)
#define Sck_ErrTimeOut				(Sck_BaseErr+2)
#define Sck_ErrPeerCloser 		(Sck_BaseErr+3)
#define Sck_ErrMalloc         (Sck_BaseErr+4)


//客户端初始化
//int sckClient_init(void **handle,char *ip,int port,int contime,int sendtime,int revtime);

//客户端初始化2
int sckClient_init(void **handle,int contime,int sendtime,int revtime,int nConNum);
//int sckClient_init(void **handle,int contime,int sendtime,int revtime);

//客户端连接
int sckClient_getconn(void *handle,char * ip,int port,int *connfd);

//客户端关闭连接
//int sckClient_putconn(int *connfd);

int sckClient_closeconn(int connfd);



//客户端发送报文
int sckClient_send(void *handle,int connfd,unsigned char *data,int datalen);
//客户端接收报文
int sckClient_rev(void *handle,int connfd,unsigned char *out,int *outlen);

//客户端释放
int sckClient_destroy(void *handle);

	
	

/*---------------------------------------------------------------------------------------------*/

//服务端初始化2
int sckServer_init(int port,int *listenfd);
int sckServer_accept(int listenfd,int *connfd,int timeout);

//服务端发送报文
int sckServer_send(int connfd,unsigned char *data,int datalen,int timeout);
//服务端接收报文
int sckServer_rev(int connfd,unsigned char *out,int *outlen,int timeout);

//服务端释放
int sckServer_destroy(int *listenfd);

	
#ifdef __cplusplus
}
#endif
#endif