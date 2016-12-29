#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/errno.h>
#include<arpa/inet.h>
#include<pthread.h>

#define MAXLINE 1024
#define SERV_PORT 9888



//定义线程特定数据的结构
typedef struct{
	int r1_cnt;
	char *r1_bufptr;
	char r1_buf[MAXLINE];
}Rline;

static pthread_key_t r1_key;//线程特定数据--键
static pthread_once_t r1_once = PTHREAD_ONCE_INIT;//保证申请线程特定数据的值 的函数只被调用一次

void *copyto(void *);
static ssize_t my_read(Rline *tsd,int fd,char *ptr);
static void readline_once(void);
static void readline_destructor(void * ptr);
ssize_t readline(int fd,void *vptr,size_t maxlen);
void str_cli(FILE *fp_arg, int socketfd_arg);

static int socketfd;
static FILE *fp;




void main(int argc,char** argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	
	if(argc != 2)
	{
		printf("argument error\n");
		exit(-1);
	}
	
	//创建套接字
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("scoket create error\n");
		exit(-1);
	}
	
	//配置套接字
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	if((inet_pton(AF_INET,argv[1],&servaddr.sin_addr))<0)
	{
		perror("pton error\n");
		exit(-1);
		
	}
	
	//连接到服务器
	if ((connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)))<0)
	{
		perror("connect error\n");
		exit(-1);
	}
	
	printf("connect to IP: %s ok\n",argv[1]);
	str_cli(stdin,sockfd);
	exit(0);
}

//客户端函数
void str_cli(FILE *fp_arg, int socketfd_arg)
{
	printf("call str_cli ok \n");
	char recvline[MAXLINE];
	pthread_t tid;

	socketfd = socketfd_arg;
	fp = fp_arg;
	
	//创建新线程，调用处理函数copyto，参数为NULL
	pthread_create(&tid, NULL, copyto, NULL);
	
	//从套接字接收服务器发回的数据，保存在recvline中，并输出
	while ((readline(socketfd, recvline, MAXLINE)) > 0)
	{
		printf("readline ok\n");
		fputs(recvline, stdout);
	}


}

//线程处理函数
void *copyto(void *arg)
{
	printf("child thread create ok\n");
	char sendline[MAXLINE];
	
	printf("please input a string\n");
	
	//将通过fp收到的信息保存在sendline中，然后通过write写回socketfd中
	while (fgets(sendline, MAXLINE, fp) != NULL)
	{
		printf("sendline : %s\n",sendline);
		write(socketfd, sendline, strlen(sendline));
	}
	
	printf("write sendline:%s ok \n",sendline);
		
	//当在标准输入读到EOF时，通过调用shutdown从套接字送出FIN
	shutdown(socketfd, SHUT_WR);
	return (NULL);
}






//以下均为采用线程特定数据 实现的readline函数的定义

//析构函数释放本线程分配的内存区
static void readline_destructor(void * ptr)
{
	free(ptr);
}

//一次性函数创建readline使用的键
static void readline_once(void)
{
	pthread_key_create(&r1_key,readline_destructor);
}


//每次最多读MAXLINE个字符，每次返回一个字符
static ssize_t my_read(Rline *tsd,int fd,char *ptr)
{
	if(tsd->r1_cnt <= 0)
	{
		again:
		if((tsd->r1_cnt = read(fd,tsd->r1_buf,MAXLINE)) <0)
		{
			if(errno == EINTR)
				goto again;
			return (-1);
		}
		else if(tsd->r1_cnt == 0)
			return (0);
		
		tsd->r1_bufptr = tsd->r1_buf;
	}
	tsd->r1_cnt--;
	*ptr = *tsd->r1_bufptr++;
	return (1);
	
}



ssize_t readline(int fd,void *vptr,size_t maxlen)
{
	size_t n,rc;
	char c,*ptr;
	Rline *tsd; //用于临时存放新分配的内存区
	
	//确保readline_once在进程氛围内只被调用一次，创建线程特定数据键，保存在r1_key中
	pthread_once(&r1_once,readline_once);
	
	if((tsd=pthread_getspecific(r1_key)) == NULL)
	{
		tsd = calloc(1,sizeof(Rline)); //将Rline的成员初始化为0
		pthread_setspecific(r1_key,tsd);
	}
	
	ptr = vptr;
	for(n = 1; n < maxlen; n++)
	{
		if((rc = my_read(tsd,fd,&c)) == 1)
		{
			*ptr++ =c;
			if(c == '\n')
				break;
		}
		else if(rc == 0 )
		{
			*ptr = 0;
			return (n-1);
		}
		else
			return (-1);
	}
	
	*ptr = 0;
	return n;	
}