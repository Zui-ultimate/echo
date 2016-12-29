#include "connect_serv.h"
#include "str_echo.h"
#include "signal_handle.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<signal.h>
#include<sys/wait.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<errno.h>

#ifndef SERV_PORT
#define SERV_PORT 9877
#endif

#ifndef MAXLINE
#define MAXLINE 1024
#endif