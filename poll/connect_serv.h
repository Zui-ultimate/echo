#ifndef _CONNECT_SERV_H_
#define _CONNECT_SERV_H_
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>

#include<netinet/in.h>
#include<sys/socket.h>


int Socket(int family, int type, int protocol);
void Bind(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);
void Listen(int sockfd,int backlog);
int Accept(int sockfd,struct sockaddr* cliaddr,socklen_t *addrlen);
void Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen);

#endif