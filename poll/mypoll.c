#include "mypoll.h"

int Poll(struct pollfd *fdarray,unsigned long nfds,int timeout)
{
	int n;
	if((n = poll(fdarray,nfds,timeout))<0)
	{
		fprintf(stderr,"poll error:%s\n",strerror(errno));
		exit(1);
	}
	return n;
}