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

#define MAXLINE 102400
#define SERV_PORT 9877
#define FDSIZE 2048
#define EPOLLEVENTS 2000

//函数声明
//创建套接字并绑定
static int socket_init(void);
//IO多路复用epoll
static void do_epoll(int listenfd);
//事件处理函数
static void handle_events(int epolfd,struct epoll_event* events,int ret_num,int listenfd,char* buf);
//处理接收到的连接
static void handle_accept(int epollfd,int listenfd);
//处理读
static void do_read(int epollfd,int sockfd,char *buf);
//处理写
static void do_write(int epollfd,int sockfd,char *buf);
//处理注册事件
void register_event(int epollfd,int option,int sockfd,int state);


int count = 0;
int main(int argc,char ** argv)
{
	int listenfd;
	listenfd = socket_init();

	do_epoll(listenfd);
	
	return 0;
}

static int socket_init(void)
{
	int listenfd;
	struct sockaddr_in servaddr;

	//创建套接字
	if((listenfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket create error");
		exit(-1);
	}
	
	//设置为非阻塞
	int flag;
	flag = fcntl(listenfd,F_GETFL);
	fcntl(listenfd,F_SETFL,flag|O_NONBLOCK);
	//printf("socket create ok\n");

	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定套接字
	if((bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		perror("bind error");
		exit(-1);
	}
	//printf("bind ok\n");
	
		
	//监听套接字
	if((listen(listenfd,2000))<0)
	{
		perror("listen error");
		exit(-1);
	}
	//printf("listen ok\n");
	
	
	return listenfd;
}

static void do_epoll(int listenfd)
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
	register_event(epollfd,EPOLL_CTL_ADD,listenfd,EPOLLIN);

	//printf("waiting for connection\n");
	for(;;)
	{
		//获取已经准备好的描述符事件
		ret = epoll_wait(epollfd,events,EPOLLEVENTS,-1);
		//printf("ret = %d\n",ret);
		handle_events(epollfd,events,ret,listenfd,buf);
		
	}
	
	close(epollfd);
	
	return ;
}

static void handle_events(int epollfd,struct epoll_event* events,int ret_num,int listenfd,char* buf)
{
	int i;
	int fd;
	//进行遍历,分别处理就绪事件
	for(i = 0 ; i < ret_num ; i++)
	{
		fd = events[i].data.fd;
		//根据描述符的类型和事件类型处理
		if((fd == listenfd) && (events[i].events & EPOLLIN))//建立新连接
			handle_accept(epollfd,listenfd);
		else if(events[i].events & EPOLLIN)
			do_read(epollfd,fd,buf);
		else if(events[i].events & EPOLLOUT)
			do_write(epollfd,fd,buf);
	}
	
	return ;
}

static void handle_accept(int epollfd,int listenfd)
{
	//printf("listenfd is:%d\n",listenfd);
	int connfd;
	struct sockaddr_in cliaddr;
	socklen_t clilen ;
	connfd = accept(listenfd,(struct sockaddr *)&cliaddr,&clilen);
	if(connfd == -1)
		perror("accept error");
	else
	{
		//printf("accept ok. client:%s:%d\n",inet_ntoa(cliaddr.sin_addr),cliaddr.sin_port);
		printf("count = %d\n",++count);
		register_event(epollfd,EPOLL_CTL_ADD,connfd,EPOLLIN);
	}
	
	return;
}


static void do_read(int epollfd,int sockfd,char *buf)
{
	//printf("doing read\n");
	int nread;
	nread = read(sockfd,buf,MAXLINE);
	if(nread <= 0)
	{
		fprintf(stderr,"client %d(socket) closed.\n",sockfd);
		close(sockfd);
		register_event(epollfd,EPOLL_CTL_DEL,sockfd,EPOLLIN);
	}
	else
	{
		printf("%s   %d\n",buf,count);
		//修改描述符对应的事件，由读改为写
		register_event(epollfd,EPOLL_CTL_MOD,sockfd,EPOLLOUT);
	}
	
	return;
}

static void do_write(int epollfd,int sockfd,char *buf)
{
	//printf("doing write\n");
	int nwrite;
	nwrite = write(sockfd,buf,strlen(buf));
	if(nwrite == -1)
	{
		perror("write error");
		close(sockfd);
		register_event(epollfd,EPOLL_CTL_DEL,sockfd,EPOLLOUT);
	}
	else
		register_event(epollfd,EPOLL_CTL_MOD,sockfd,EPOLLIN);
	memset(buf,0 ,MAXLINE);
	
	return ;
}

void register_event(int epollfd,int option,int sockfd,int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = sockfd;
	
	epoll_ctl(epollfd,option,sockfd,&ev);
	
}


