#include "io.h"

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


ssize_t Read(int sockfd,void *vptr,size_t nbytes)
{
    ssize_t n;  
    if ((n = read(sockfd, vptr, nbytes)) == -1)
	{
		fprintf(stderr,"read error:%s\n",strerror(errno));
		return -1;
	}
    return n;
}