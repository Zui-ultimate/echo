#ifndef _IO_H_
#define _IO_H_

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>


ssize_t Writen(int sockfd,const void *vptr,size_t n);
ssize_t Read(int sockfd,void *vptr,size_t n);

#endif