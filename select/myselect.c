#include "myselect.h"

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{   
    int  n;  
    if ( (n = select(nfds, readfds, writefds, exceptfds, timeout)) < 0)
	{
		fprintf(stderr,"select error:%s\n",strerror(errno));
		exit(1);
	}
    return n;     
}