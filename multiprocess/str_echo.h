#ifndef _STR_ECHO_H_
#define _STR_ECHO_H_

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>


#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<unistd.h>
#endif

#ifndef MAXLINE
#define MAXLINE 1024


void str_echo(int sockfd);
ssize_t Writen(int sockfd,const void *vptr,size_t n);
ssize_t Read(int sockfd,void *vptr,size_t n);

#endif