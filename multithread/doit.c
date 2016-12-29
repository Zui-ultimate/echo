#include "doit.h"
void * doit(void *arg)
{
	//printf("child thread create ok\n");
	
	int connfd;
	
	connfd = *((int*)arg);
	free(arg);
	
	Pthread_detach(pthread_self());//线程自身脱离，主线程不必等待它所创建的每个线程
	str_echo(connfd);
	close(connfd);//在子线程关闭连接的套接字， 因为此时主线程、子线程同时在运行，且共享所有描述符，主线程无法找到子线程的运行情况。
	return (NULL);
}
