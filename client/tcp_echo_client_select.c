#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

#define MAXLINE 1024
#define SERV_PORT 9877

#define data_in "test.txt" //��������Դ

int max(int a ,int b);
void str_cli(FILE *fp ,int sockfd);

void main(int argc ,char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	
	FILE *fp = fopen(data_in,"r+");
	
	if(argc != 2)
	{
		printf("argument error,IP address needed\n");
		exit(-1);
	}
	
	//�����׽���
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)
	{
		perror("socket create error");
		exit(-1);
	}
	
	printf("socket create ok\n");
	
	//�����׽���
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	
	//������ip��ַת����Ϊ�����ַ
	inet_pton(AF_INET,argv[1],&servaddr.sin_addr);
	
	//�����׷�����
	if((connect(sockfd,(struct sockaddr*)&servaddr,sizeof(servaddr)))<0)
	{
		perror("connect error");
		exit(-1);
	}
	
	printf("connect to %s:%d ok\n",argv[1],SERV_PORT);
	
	//���ÿͻ������ӳ���
	//str_cli(fp,sockfd);
	str_cli(stdin,sockfd);
	
	exit(0);
}

void str_cli(FILE *fp,int sockfd)
{
	int maxfdp1;//���������+1
	int stdineof;//�յ�EOF�ı�־λ
	fd_set rset;//���ļ�����������
	char buf[MAXLINE];
	int n;//��¼���������ݵĸ���
	
	stdineof = 0;
	
	//��ʼ�����ļ���������
	FD_ZERO(&rset);
	
	for(;;)
	{
		if(stdineof == 0)
			FD_SET(fileno(fp),&rset);//����׼�ļ�����λ�򿪣�fp����fileno�ǽ�fpָ��ת��Ϊ��Ӧ������������Ϊselectֻ�ܲ���������
		FD_SET(sockfd,&rset);//���׽���λ��
		
		maxfdp1 = max(fileno(fp),sockfd) + 1;//���������������
		select(maxfdp1,&rset,NULL,NULL,NULL);
		
		//���select���ص����׽���sockfd�ɶ�
		if(FD_ISSET(sockfd,&rset))
		{
			//���û�ж�������
			if((n = read(sockfd,buf,MAXLINE)) == 0)/*�������ı���Ϊ���ģ�������Ի�����buf����*/
			{
				if(stdineof == 1)
					return;//����������ĩβ�������˳�
				else
				{
					printf("server terminated prematurely\n");
					exit(-1);
				}
			}
			
			write(fileno(stdout),buf,n);//����׼���ת��������������Ϊselectֻ�ܲ���������
		}
		
		//���select���ص��Ǳ�׼����stdin�ɶ�
		if(FD_ISSET(fileno(fp),&rset))
		{
			//���û�ж�������
			if((n = read(fileno(fp),buf,MAXLINE)) == 0)/*�������ı���Ϊ���ģ�������Ի�����buf����*/
			{
				stdineof = 1;//��EOFλ��1����ʾ�Ѿ�������ĩβ
				shutdown(sockfd,SHUT_WR);
				FD_CLR(fileno(fp),&rset);
				continue;
			}
			
			write(sockfd,buf,n);/*�������ı���Ϊ���ģ�������Ի�����buf����*/
		}
	}
	
}

int max(int a,int b)
{
	if(a>=b)
		return a;
	else
		return b;
	
}























