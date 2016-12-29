#include "main.h"

int main(int argc ,char ** argv)
{
	int count = 0;
	int listenfd,connfd,sockfd;
	struct sockaddr_in servaddr,cliaddr;
	socklen_t clilen;

	char buf[MAXLINE];
	int maxi,maxfd,i;//client[]当前最大索引，select最大描述符数
	ssize_t n;//存储已读的个数
	fd_set rset,allset;//读文件描述符集、临时描述符集
	int nready;//已就绪描述符个数
	int client[FD_SETSIZE];//存储新的已连接描述符
	
	//创建套接字
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	
	//printf("socket create ok\n");

	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	//绑定套接字
	Bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr));
	//printf("bind ok\n");

	//监听套接字
	Listen(listenfd,10000);
	//printf("listen ok\n");

	maxfd = listenfd;//初始化最大描述符数，为监听的套接字
	maxi = -1;//初始化client[]数组索引

	//初始化client[]数组，值为-1
	for(i = 0; i < FD_SETSIZE; i++)
		client[i] = -1;

	FD_ZERO(&allset);//allset集 初始化为0
	FD_SET(listenfd,&allset);//打开 监听套接字 位

	for(;;)
	{
		rset = allset;
		nready = Select(maxfd+1,&rset,NULL,NULL,NULL);

		//如果有新连接建立
		if(FD_ISSET(listenfd,&rset))
		{
			clilen = sizeof(cliaddr);
			//连接客户端
			connfd = Accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);

			//将客户端网络地址转换为主机地址
			/*char clientName[INET_ADDRSTRLEN];
			inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,clientName,sizeof(clientName));
			printf("client connected. IP %s:%d\n",clientName,ntohs(cliaddr.sin_port));
			printf("count = %d\n",++count);*/

			//将新建立的connfd存放到client[]第一个可用的位置
			for(i = 0 ; i < 1024; i++)
				if(client[i]<0)
				{
					client[i] = connfd;
					break;
				}
			if(i == FD_SETSIZE)
			{
				fprintf(stderr,"too many clients\n");
				exit(-1);
			}

			//将新建立的connfd添加到allset中，之后再赋给rset
			FD_SET(connfd,&allset);

			if(connfd>maxfd)
				maxfd = connfd;//更新最大描述符数
			if(i>maxi)
				maxi = i;//改变client[]的当前最大索引值

			//若没有就绪的描述符，则再次循环调用select
			if(--nready <= 0)
				continue;
		}

		for(i = 0; i <= maxi; i++)
		{
			if((sockfd = client[i])<0)
				continue;
			if(FD_ISSET(sockfd,&rset))
			{
				if((n = Read(sockfd,buf,MAXLINE)) == 0)//如果没有读到数据,则本客户关闭连接
				{
					close(sockfd);
					FD_CLR(sockfd,&allset);//将select中，该客户端的位置为0
					client[i] = -1;//将索引为i的cilent数字，置为-1.证明未使用
				}
				else
				{
					//printf("%s\n",buf);
					Writen(sockfd,buf,n);
					//printf("write back to client ok\n");
				}

				if(--nready <= 0)
					break;//没有可读的描述符
					
			}
		}

	}

				
}
