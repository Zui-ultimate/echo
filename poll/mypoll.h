#ifndef _MYPOLL_H_
#define _MYPOLL_H_

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<sys/poll.h>

int Poll(struct pollfd *fdarray,unsigned long nfds,int timeout);

#endif
