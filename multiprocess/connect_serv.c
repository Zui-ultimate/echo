#include "connect_serv.h"

int Accept(int sockfd,struct sockaddr* cliaddr,socklen_t *addrlen)
{
	int	n;

again:
	if ( (n = accept(sockfd, (struct sockaddr*)&cliaddr, addrlen)) < 0) {
#ifdef	EPROTO
		if (errno == EPROTO || errno == ECONNABORTED)
#else
		if (errno == ECONNABORTED)
#endif
			goto again;
		else
		{
			perror("accept error");
			exit(1);
		}
	}
	return(n);
}

void Bind(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
	if (bind(sockfd,servaddr,addrlen) < 0)
		{
			perror("bind error");
			exit(1);
		}
}

int Socket(int family, int type, int protocol)
{
	int	n;

	if ( (n = socket(family, type, protocol)) < 0)
		{
			perror("socket error");
			exit(1);
		}
	return(n);
	
}


void Listen(int fd, int backlog)
{
	if (listen(fd, backlog) < 0)
		{
			perror("listen error");
			exit(1);
		}
}

void Connect(int sockfd, const struct sockaddr *servaddr, socklen_t addrlen)
{
	if (connect(sockfd, servaddr, addrlen) < 0)
	{
		perror("connect error");
		exit(1);
	}
}
