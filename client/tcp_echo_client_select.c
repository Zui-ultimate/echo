#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

#define MAXLINE 1024
#define SERV_PORT 9877

#define data_in "test.txt" //输入数据源

int max(int a ,int b);
void str_cli(FILE *fp ,int sockfd);

void main(int argc ,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	
	FILE *fp = fopen(data_in,"r+");
	
	if(argc != 2)
	{
		printf("argument error,IP address needed\n");
		exit(-1);
	}
	
	//创建套接字
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket create error");
		exit(-1);
	}
	
	printf("socket create ok\n");
	
	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	
	//将本地ip地址转换成为网络地址
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	
	//连接套服务器
	if((connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		perror("connect error");
		exit(-1);
	}
	
	printf("connect to %s:%d ok\n",argv[1],SERV_PORT);
	
	//调用客户服务子程序
	//str_cli(fp,sockfd);
	str_cli(stdin,sockfd);
	
	exit(0);
}

void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1;//最大描述符+1
	int stdineof;//收到EOF的标志位
	fd_set rset;//读文件描述符集合
	char buf[MAXLINE];
	int n;//记录读到的数据的个数
	
	stdineof = 0;
	
	//初始化读文件描述符集
	FD_ZERO(&rset);
	
	for(;;)
	{
		if(stdineof == 0)
			FD_SET(fileno(fp),&rset);//将标准文件输入位打开（fp），fileno是将fp指针转换为相应的描述符，因为select只能操作描述符
		FD_SET(sockfd,&rset);//将套接字位打开
		
		maxfdp1 = max(fileno(fp),sockfd) + 1;//设置最大描述符数
		select(maxfdp1,&rset,NULL,NULL,NULL);
		
		//如果select返回的是套接字sockfd可读
		if(FD_ISSET(sockfd,&rset))
		{
			//如果没有读到数据
			if((n = read(sockfd,buf,MAXLINE)) == 0)/*不再以文本行为中心，而是针对缓冲区buf操作*/
			{
				if(stdineof == 1)
					return;//读到了数据末尾，正常退出
				else
				{
					printf("server terminated prematurely\n");
					exit(-1);
				}
			}
			
			write(fileno(stdout),buf,n);//将标准输出转换成描述符，因为select只能操作描述符
		}
		
		//如果select返回的是标准输入stdin可读
		if(FD_ISSET(fileno(fp),&rset))
		{
			//如果没有读到数据
			if((n = read(fileno(fp),buf,MAXLINE)) == 0)/*不再以文本行为中心，而是针对缓冲区buf操作*/
			{
				stdineof = 1;//将EOF位置1，表示已经读到了末尾
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			
			write(sockfd,buf,n);/*不再以文本行为中心，而是针对缓冲区buf操作*/
		}
	}
	
}

int max(int a,int b)
{
	if(a>=b)
		return a;
	else
		return b;
	
}























