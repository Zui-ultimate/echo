#include "doit.h"
void * doit(void *arg)
{
	//printf("child thread create ok\n");
	
	int connfd;
	
	connfd = *((int*)arg);
	free(arg);
	
	Pthread_detach(pthread_self());//�߳��������룬���̲߳��صȴ�����������ÿ���߳�
	str_echo(connfd);
	close(connfd);//�����̹߳ر����ӵ��׽��֣� ��Ϊ��ʱ���̡߳����߳�ͬʱ�����У��ҹ������������������߳��޷��ҵ����̵߳����������
	return (NULL);
}
