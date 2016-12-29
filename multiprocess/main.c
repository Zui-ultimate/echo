#include "main.h"

int main(int argc, char **argv)
{
        int listenfd, connfd;
        pid_t childpid;
        socklen_t clilen;
        struct sockaddr_in cliaddr, servaddr;
	
		//创建套接字
        listenfd = Socket(AF_INET, SOCK_STREAM, 0);
  
		//配置服务器socket
        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(SERV_PORT);

		//printf("sock create ok\n");
				
		//将套接字bind在熟知端口上
        Bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
   		//printf("bind ok\n");
		
		//监听端口
        Listen(listenfd, 10000);
		
		//处理僵死进程
		Signal(SIGCHLD,sig_chld);
		
		//printf("waiting for connect\n");
        for (;;)
        {
				//保存客户端地址信息
                clilen = sizeof(cliaddr);
				
				//获取客户端套接字
                connfd = Accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
				//printf("connected ok\n");
				
				//获取客户端IP和port
				char clientName[INET_ADDRSTRLEN];
				inet_ntop(AF_INET,&cliaddr.sin_addr.s_addr,clientName,sizeof(clientName));
						
				//服务器子程序处理数据
                if ((childpid = fork()) == 0)
                {
                        close(listenfd);
						//printf("child process closes listenfd ok\n");
                        str_echo(connfd);
						//printf("call str_echo\n");
                        exit(0);
                }
                close(connfd);
        }

}



