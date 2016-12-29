#include "connect_serv.h"
#include "io.h"
#include "myselect.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>
#include<unistd.h>

#define SERV_PORT 9877
#define MAXLINE 1024
