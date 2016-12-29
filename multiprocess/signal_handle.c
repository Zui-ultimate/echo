#include "signal_handle.h"

sighandler_t Signal(int signo, sighandler_t func)
{
	void *sig;
	
	if((sig = signal(signo,func)) == SIG_ERR)
	{
		perror("signal error");
		exit(1);
	}
	return (sig);
}

void sig_chld(int signo)
{
	pid_t pid;
	int stat;
	
	//pid = wait(&stat);
	while((pid = waitpid(-1,&stat,WNOHANG)) > 0)
		//printf("child %d terminated\n",pid);
	return;
}
