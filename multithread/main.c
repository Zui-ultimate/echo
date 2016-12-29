#include "main.h"

int main(int argc, char ** argv)
{
	int listenfd, *iptr;
	pthread_t tid;
	socklen_t clilen;
	struct sockaddr_in cliaddr,servaddr;

	//创建套接字
	listenfd=Socket(AF_INET,SOCK_STREAM,0);


	//printf("socket create ok\n");

	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	Bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));

	//printf("bind ok\n");

	//监听套接字
	Listen(listenfd,10000);
	//printf("listen ok\n");
	//printf("waiting for connect\n");

	for(;;)
	{
		clilen = sizeof(cliaddr);
		iptr = malloc(sizeof(int));


		//获取客户端套接字
		*iptr = Accept(listenfd,(struct sockaddr*)&cliaddr,&clilen);

		//printf("connect ok\n");

		//获取客户端ip和端口号
		/*char clilentName[INET_ADDRSTRLEN];
		if(inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,clilentName,sizeof(clilentName))!= NULL)
		{
			printf("connected by clilent:%s:%d\n",clilentName,ntohs(cliaddr.sin_port));
		}*/

		//创建子线程处理数据
		Pthread_create(&tid,NULL,&doit,iptr);

	}
}
