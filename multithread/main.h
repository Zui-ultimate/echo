#include "connect_serv.h"
#include "thread_handle.h"
#include "str_echo.h"
#include "doit.h"
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<errno.h>
#include<netinet/in.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<string.h>

#define SERV_PORT 9877
