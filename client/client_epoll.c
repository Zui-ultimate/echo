/*
*参数说明：
*argv[1] server_ip
*argv[2] 链接数
*argv[3] 每个连接发送的数据次数
*argv[4] port
*argv[5] 每执行n次epoll_wait，输出一次平均并发数
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<unistd.h>

#define FDSIZE 102400
#define EPOLLEVENT 102400
#define MAXLINE 1024

static int num;

int timeval_subtract(struct timeval* result, struct timeval* x, struct timeval* y) 
  { 
        int nsec; 
    
        if ( x->tv_sec>y->tv_sec ) 
                  return -1; 
    
        if ( (x->tv_sec==y->tv_sec) && (x->tv_usec>y->tv_usec) ) 
                  return -1; 
    
        result->tv_sec = ( y->tv_sec-x->tv_sec ); 
        result->tv_usec = ( y->tv_usec-x->tv_usec ); 
    
        if (result->tv_usec<0) 
        { 
            result->tv_sec--; 
            result->tv_usec+=1000000; 
        } 
    
        return 0; 
  }
  
  
int register_event(int epollfd,int option,int sockfd,int state)
{
	struct epoll_event ev;
	ev.events = state;
	ev.data.fd = sockfd;
	
	if(epoll_ctl(epollfd,option,sockfd,&ev) == -1)
	{
		fprintf(stderr,"epoll_ctl error: %s\n",strerror(errno));
		return -1;
	}
	
}

int setnoblock(int sockfd)
{
	int flag;
	if((flag = fcntl(sockfd,F_GETFL,0)) < 0)
	{
		fprintf(stderr,"get sockfd flag error:%s\n",strerror(errno));
		return -1;
	}
	if((fcntl(sockfd,F_SETFL,flag|O_NONBLOCK)) < 0)
	{
		fprintf(stderr,"set sockfd flag error:%s\n",strerror(errno));
		return -1;
	}
	return 0;
}



int create_conn(struct sockaddr_in servaddr,int epollfd)
{	
	int sockfd;
	
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		fprintf(stderr,"socket create error:%s\n",strerror(errno));
		return -1;
	}
	
	if((connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		if(errno == EINPROGRESS)
		{
			num++;
			setnoblock(sockfd);
			register_event(epollfd,EPOLL_CTL_ADD,sockfd,EPOLLOUT);
			printf("current connection:%d\n",num);
			return 0;
		}
		fprintf(stderr,"connect error:%s\n",strerror(errno));
		close(sockfd);
		exit(1);
	}
	num++;
	
	//设为非阻塞
	setnoblock(sockfd);
	
	//注册到epoll
	register_event(epollfd,EPOLL_CTL_ADD,sockfd,EPOLLOUT);
	printf("num == %d\n",num);
		
}


int main(int argc,char ** argv)
{
	int currency = atoi(argv[2]);//并发数
	int total;
	int readed = 0;
	int wait_times = 0;
	total = currency * atoi(argv[3]);//请求数
	
	if(argc != 6)
	{
		fprintf(stderr,"number of arguments error\n");
		return -1;
	}
	
	struct sockaddr_in servaddr;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(atoi(argv[4]));//端口
	inet_pton(AF_INET,argv[1],(struct sockaddr*)&servaddr.sin_addr);
	
	
	
	int epollfd ;
	if((epollfd = epoll_create(FDSIZE)) < 0)
	{
		fprintf(stderr,"epoll create error:%s\n",strerror(errno));
		return -1;
	}
	struct epoll_event events[EPOLLEVENT];
	
	//初始化并发链接数
	int i = 0;
	for(i;i<currency;i++)
		create_conn(servaddr,epollfd);

	struct timeval start,stop,diff;//计时
	
	gettimeofday(&start,0);
	
	for(;;)
	{
		//printf("1\n");
		int ret;
		ret = epoll_wait(epollfd,events,FDSIZE,-1);
		for(i=0;i<ret;i++)
		{
			if(events[i].events & EPOLLOUT)
			{
				int fd = events[i].data.fd;
				char *buffer_write = "hello";
				int nwrite = write(fd,buffer_write,sizeof(buffer_write)+1);
				if(nwrite < 0)
				{
					if(errno != EWOULDBLOCK)
					{
						fprintf(stderr,"write error:%s\n",strerror(errno));
						return -1;
					}
				}
				if(nwrite > 0 )
				{
					register_event(epollfd,EPOLL_CTL_MOD,fd,EPOLLIN);
				}			
			}
			
			else if(events[i].events & EPOLLIN)
			{
				int fd = events[i].data.fd;
				char buffer_read[MAXLINE];
				int nread = read(fd,buffer_read,MAXLINE);
				buffer_read[nread] = '\0';
				if(nread < 0)
				{
					if(errno != EWOULDBLOCK)
					{
						fprintf(stderr,"read error:%s\n",strerror(errno));
						return -1;
					}
				}
				
				if(strcmp(buffer_read,"hello") != 0)
					fprintf(stderr,"not match!\n");
				
				if(nread > 0 )
				{
					register_event(epollfd,EPOLL_CTL_MOD,fd,EPOLLOUT);
				}
				readed++;			
			}
		}
		
		wait_times++;
		if(wait_times > atoi(argv[5]))
				{
					gettimeofday(&stop,0);
					timeval_subtract(&diff,&start,&stop);
					printf("readed = %d\t currency now:%lf\n",readed,(readed)/(diff.tv_sec+diff.tv_usec/(double)1000000));
					wait_times = 0;
				}
		if(readed > total)
				{
					printf("start_time:%ld.%ld\n",start.tv_sec,start.tv_usec);
					gettimeofday(&stop,0);
					printf("stop_time:%ld.%ld\n",stop.tv_sec,stop.tv_usec);
					timeval_subtract(&diff,&start,&stop);
					printf("run time:%ld.%ld\n",diff.tv_sec,diff.tv_usec);
					
					printf("count = %d\n",total);
														
					printf("currency:%lf\n",(total)/(diff.tv_sec+diff.tv_usec/(double)1000000));
					
					close(epollfd);
					return 0;
				}
		
	}
	
}



