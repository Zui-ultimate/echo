#include "main.h"

int main(int argc,char ** argv)
{
	int sockfd,listenfd,connfd;
	char buf[MAXLINE];//数据缓冲区
	struct sockaddr_in servaddr,cliaddr;
	socklen_t clilen;
	int nready;//已准备好的描述符个数
	int n;//已读入的数据的个数
	struct pollfd client[OPEN_MAX];//记录客户已连接套接字描述符
	int i;//作为for循环的临时变量
	int maxi;//client数组当前使用的最大下标值
	char clientName[2048][INET_ADDRSTRLEN];//存储已连接客户端的ip
	//char *clientName(sizeof(char)*INET_ADDRSTRLEN);//存储已连接客户端的ip

	//创建作为监听的套接字
	listenfd = Socket(AF_INET,SOCK_STREAM,0);
	//printf("socket create ok\n");

	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	Bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	//printf("bind ok\n");

	//监听套接字
	Listen(listenfd,MAXLINE);
	//printf("listen ok\n");
	
	//初始化poll
	client[0].fd = listenfd;//把client数组的第一项用于监听套接字
	client[0].events = POLLRDNORM;//设置需要监听的事件，当有新连接准备好被接受时，poll将通知
	for(i = 1 ; i < OPEN_MAX ; i++)//将client数组的其余描述符成员置为-1.表示未使用
		client[i].fd = -1;
	maxi = 0;//client数组当前使用的最大下标值

	//调用poll，检查新的连接
	for(; ;)
	{
		nready = Poll(client,maxi+1,INFTIM);//返回已就绪的描述符个数

		//如果有新客户端连接
		if(client[0].revents & POLLRDNORM)
		{
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);
			//printf("accept ok\n");
			
			//从cilent数组中，找到第一个描述符成员为负的可用项
			for(i = 1 ; i < OPEN_MAX ; i++)
			{
				if(client[i].fd < 0)
				{
					client[i].fd = connfd;//保存已连接套接字描述符
					
					//输出已连接的客户端的信息
					//inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,clientName[i],sizeof(cliaddr));
					//printf("client connected. IP %s:%d\n",clientName[i],cliaddr.sin_port);
					break;
				}
			}

			if(i == OPEN_MAX)
			{
				fprintf(stderr,"too many clients\n");
				return -1;
			}

			client[i].events = POLLRDNORM;//将新保存的套接字项监听的对象 置为：POLLRDNORM
			
			//更新maxi
			if(i>maxi)
				maxi =i;
			
			//若没有已就绪套接字描述符，则continue
			if(--nready <= 0)
				continue;
		}

		//检查某个现有连接上的数据
		for(i = 1 ; i < maxi ; i++)
		{
			//在client数组中找到已设置connfd的项，即client[i]值不为-1
			if((sockfd = client[i].fd)<0)
				continue;

			if(client[i].revents & (POLLRDNORM | POLLERR))
			{
				if((n = read(sockfd,buf,MAXLINE))<0)
				{
					//如果客户端重置连接
					if(errno == ECONNRESET)
					{
						close(sockfd);
						client[i].fd = -1;
					}
					else
					{
						fprintf(stderr,"read error\n");
						return -1;
					}
				}
				//客户端关闭连接
				else if(n == 0)
				{
					close(sockfd);
					client[i].fd = -1;
				}
				else
				{
					//printf("receive message from IP %s  :%s\n",clientName[i],buf);
					//printf("%s\n",buf);
					Writen(sockfd,buf,n);
					//printf("write back ok\n");
				}

				if(--nready <= 0)
					break;
			}
		}

	}
		
}
