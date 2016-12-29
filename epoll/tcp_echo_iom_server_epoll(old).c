/*************************************************************************
    > File Name: tcp_echo_ioMserver_epoll.c
    > Author: sunyue
    > Created Time: Thu 03 Nov 2016 04:45:08 PM CST
 ************************************************************************/
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<fcntl.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>

#define MAXLINE 1024
#define SERV_PORT 9877
#define FDSIZE 2048
#define EPOLLEVENTS 2000

//函数声明
//创建套接字并绑定
static int socket_init(void);
//非阻塞
static int setnoblock(int sockfd);


static int do_epoll(int listenfd);
//事件处理函数
static int handle_events(int epolfd,struct epoll_event* events,int ret_num,int listenfd,char* buf);
//处理接收到的连接
static int handle_accept(int epollfd,int listenfd);
//处理读
static void do_read(int epollfd,int sockfd,char *buf);
//处理写
//static int do_write(int epollfd,int sockfd,char *buf);
//处理注册事件
static int register_event(int epollfd,int option,int sockfd,int state);


int count = 0;
int main(int argc,char ** argv)
{
	int listenfd;
	if((listenfd = socket_init()) < 0)
	{
		fprintf(stderr,"listenfd init error\n");
		return -1;
	}

	if(do_epoll(listenfd) < 0)
	{
		fprintf(stderr,"do_epoll error\n");
		return -1;
	}
	
	return 0;
}

static int setnoblock(int sockfd)
{
	int flag = fcntl(sockfd,F_GETFL,0);
	if(flag < 0)
	{
		fprintf(stderr,"get flag error:%s\n",strerror(errno));
		return -1;
	}
	if(fcntl(sockfd,F_SETFL,flag|O_NONBLOCK)<0)
	{
		fprintf(stderr,"set flag error:%s\n",strerror(errno));
		return -1;
	}
	return 0;
}

static int socket_init(void)
{
	int listenfd;
	struct sockaddr_in servaddr;

	//创建套接字
	if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		fprintf(stderr,"socket create error:%s\n",strerror(errno));
		return(-1);
	}
	
	//设置为非阻塞
	if(setnoblock(listenfd) != 0)
	{
		fprintf(stderr,"set flag error\n");
		return -1;
	}
	printf("socket create ok\n");

	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	if((bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		fprintf(stderr,"bind error:%s\n",strerror(errno));
		return -1;
	}
	printf("bind ok\n");
	
		
	//监听套接字
	if((listen(listenfd,2000))<0)
	{
		fprintf(stderr,"listen error:%s\n",strerror(errno));
		return -1;
	}
	printf("listen ok\n");
	
	
	return listenfd;
}

static int do_epoll(int listenfd)
{
	int epollfd;
	struct epoll_event events[EPOLLEVENTS];
	int ret;
	char buf[MAXLINE];

	//清空buf
	memset(buf,0,MAXLINE);

	//创建epoll描述符
	epollfd = epoll_create(FDSIZE);
	
	//注册需要监听的事件
	if(register_event(epollfd,EPOLL_CTL_ADD,listenfd,EPOLLIN)<0)
	{
		fprintf(stderr,"epoll_ctl error\n");
		return -1;
	}

	//printf("waiting for connection\n");
	for(;;)
	{
		//获取已经准备好的描述符事件
		ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
		//printf("ret = %d\n",ret);
		if(handle_events(epollfd,events,ret,listenfd,buf) < 0 )
		{
			fprintf(stderr,"handle_events error\n");
			return -1;
		}
		
	}
	
	close(epollfd);
	
	return 0;
}

static int handle_events(int epollfd,struct epoll_event* events,int ret_num,int listenfd,char* buf)
{
	int i;
	int fd;
	//进行遍历,分别处理就绪事件
	for(i = 0 ; i < ret_num ; i++)
	{
		fd = events[i].data.fd;
		//根据描述符的类型和事件类型处理
		if((fd == listenfd) && (events[i].events & EPOLLIN))//建立新连接
			if(handle_accept(epollfd,listenfd)<0)
			{
				fprintf(stderr,"handle accept error\n");
				return -1;
			}
		else if(events[i].events & EPOLLIN)
			do_read(epollfd,fd,buf);
			if(do_read(epollfd,fd,buf) < 0)
			{
				fprintf(stderr,"do_read error\n");
				return -1;
			}
		else if(events[i].events & EPOLLOUT)
			if(do_write(epollfd,fd,buf) < 0 )
			{
				fprintf(stderr,"do_write error\n");
				return -1;
			}
		else if(events[i].events & EPOLLERR)
		{
			fprintf(stderr,"close socket\n");
			close(events[i].data.fd);
		}
		else
			fprintf(stderr,"!!!!!!!!!!!!!!ret:epoll_wait\n");
	}
	
	return 0;
}

static int handle_accept(int epollfd,int listenfd)
{
	printf("listenfd is:%d\n",listenfd);
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen ;
	connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
	if(connfd == -1)
	{
		if(errno == EINTR || errno == EWOULDBLOCK)
			return 0;
		fprintf(stderr,"accpet error%s\n",strerror(errno));
		return -1;
	}
	else
	{
		printf("accept ok. client:%s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
		//printf("count = %d\n",++count);
		if(setnoblock(connfd) < 0)
		{
			fprintf(stderr,"set flag error\n");
			return -1;
		}
		if(register_event(epollfd,EPOLL_CTL_ADD,connfd,EPOLLIN) < 0)
		{
			fprintf(stderr,"epoll ctl error\n");
			return -1;
		}
	}
	
	return 0;
}


static int do_read(int epollfd,int sockfd,char *buf)
{
	printf("doing read\n");
	int nread;
	printf("sockfd = %d\n",sockfd);
	nread = read(sockfd,buf,MAXLINE);
	if(nread < 0)
	{
		if(errno != EAGAIN)
		{
			fprintf(stderr,"read error:%s\n",strerror(errno));
			close(sockfd);
			return -1;
		}
	}
	else if(nread == 0)
		close(sockfd);
	else
	{
		//printf("%s   %d\n",buf,count);
		//修改描述符对应的事件，由读改为写
		register_event(epollfd,EPOLL_CTL_MOD,sockfd,EPOLLOUT);
	}
	
	return 0;
}


static int do_write(int epollfd,int sockfd,char *buf)
{
	printf("doing write\n");
	int nwrite;
	nwrite = write(sockfd,buf,strlen(buf));
	if(nwrite == -1)
	{
		fprintf(stderr,"write error:%s\n",strerror(errno));
		close(sockfd);
		return -1;
	}
	else
		register_event(epollfd,EPOLL_CTL_MOD,sockfd,EPOLLIN);
	
	memset(buf , 0 ,MAXLINE);
	
	return 0;
}

static int register_event(int epollfd,int option,int sockfd,int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = sockfd;
	
	if(epoll_ctl(epollfd,option,sockfd,&ev) == -1)
	{
		fprintf(stderr,"epoll_ctl error: %s\n",strerror(errno));
		return -1;
	}
	
	return 0;
	
}


