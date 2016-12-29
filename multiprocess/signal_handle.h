#ifndef _SIGNAL_HANDLE_H_
#define _SIGNAL_HANDLE_H_

#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<signal.h>
#include<sys/wait.h>

typedef	void (*sighandler_t)(int);	//为简化signal函数

sighandler_t Signal(int signo, sighandler_t func);//包装signal函数
void sig_chld(int signo);//sighandler_t 的信号处理函数

#endif

