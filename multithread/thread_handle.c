#include "thread_handle.h"

void Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine) (void *), void *arg)
{
	int 	n;
	if( (n = pthread_create(thread, attr, start_routine, arg)) == 0)
		return ;
	fprintf(stderr,"pthread error:%s\n",strerror(errno));
	exit(1); 
}

void Pthread_detach(pthread_t thread)
{
    int     n;  
    if ( (n = pthread_detach(thread)) == 0)
        return;
    fprintf(stderr,"pthread error:%s\n",strerror(errno));
	exit(1);
}