#include "str_echo.h"

void str_echo(int sockfd)
{
        ssize_t n;
        char buf[MAXLINE];

again:
		//接收客户端数据
        while ((n = read(sockfd, buf, MAXLINE)) > 0)
		{
			//printf("%s\n",buf);
			
			//将数据写回客户端
			Writen(sockfd, buf, n);
			//printf("str_echo write ok\n");
		}
        if (n < 0 && errno == EINTR)
                goto again;
        else if (n < 0)
		{
			fprintf(stderr,"str_echo:read error:%s\n",strerror(errno));
			exit(1);
		}

}

ssize_t Writen(int sockfd,const void *vptr,size_t n)
{
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;
	
	ptr = vptr;
	nleft = n;
	
	while(nleft > 0)
	{
		if((nwritten = write(sockfd,ptr,nleft)) <= 0)
		{
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1;
		}
		nleft -=nwritten;
		ptr += nwritten;
	}
	return (n);
}

