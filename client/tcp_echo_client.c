#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<errno.h>

#define SERV_PORT 9877
#define MAXLINE 1024


void str_cli(FILE *fp, int sockfd);
ssize_t readline(int fd,void *vptr,size_t maxlen);

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	if (argc != 2)
	{
		printf("arg error\n");
		exit(-1);
	}
	
	//分配客户端socket
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket error\n");
		exit(-1);
	}

	//配置客户端socket
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);

	//将本机地址转换成网络地址
	if ((inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) < 0)
	{
		printf("pton error\n");
		exit(-1);
	}

	
	//连接服务器，开始tcp三次握手
	if ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))<0)
	{
		printf("connect error\n");
		printf("%d\n",errno);
		exit(-1);
	}
	
	//发送数据，阻塞模式
	str_cli(stdin, sockfd);
	exit(0);

}

void str_cli(FILE *fp,int sockfd)
{
	char sendline[MAXLINE],recvline[MAXLINE];
	while((fgets(sendline,MAXLINE,fp))!=NULL)
	{
		//向服务器写数据
		write(sockfd,sendline,sizeof(sendline));
		
		//从服务器接收数据
		if((readline(sockfd,recvline,MAXLINE)) ==0)
		{
			printf("server terminated prematurely\n");
			printf("%d",errno);
		}
		
		fputs(recvline,stdout);
	}

}

ssize_t readline(int fd,void *vptr,size_t maxlen)
{
	ssize_t n,rc;
	char c,*ptr;
	
	ptr = vptr;
	for(n=1;n<maxlen;n++)
	{
		again:
		if(((rc = read(fd,&c,1)))==1)
		{
			*ptr++ =c;
			if(c == '\n')
				break;
		}
		else if(rc ==0)
		{
			*ptr = 0;
			return (n-1);
		}
		else{
			if(errno == EINTR)
				goto again;
			return (-1);
		}
	}
	
	*ptr = 0;
	return (n);
	
	
	
}
