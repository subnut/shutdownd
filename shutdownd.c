/* This program conforms to POSIX-1.2017 */
#define _POSIX_C_SOURCE 200809L


#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>


/*
 * fcntl.h	-	open()
 * signal.h	-	signal(), SIG_IGN, SIGCHLD
 * stdio.h	-	perror(), puts()
 * stdlib.h	-	EXIT_{SUCCESS,FAILURE}
 * string.h	-	strlen()
 * unistd.h	-	fork(), sleep()
 * arpa/ineth.h	-	htonl(), htons()
 */


/* man 7 tcp */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>


#include "config.h"


const char reply_200[] = "HTTP/1.0 200 OK\r\n";
const char reply_ContentType_HTML[] = "Content-Type: text/html\r\n";
const char terminate_headers[] = "\r\n";


int
main(void)
{
	/* Don't let zombies be created */
	signal(SIGCHLD, SIG_IGN);

	int sockfd;
	struct sockaddr_in address;

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons((unsigned short)585);

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	int reuseaddr = 1;
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)) == -1)
		return perror("Could not set socket options"),
		       EXIT_FAILURE;

	if (bind(sockfd, (struct sockaddr *)&address, (socklen_t)sizeof(address)) == -1)
		return perror("Could not bind"),
		       EXIT_FAILURE;

	if (listen(sockfd, SOMAXCONN) != 0)
		return perror("Could not listen"),
		       EXIT_FAILURE;

	puts("Listening...");

	for (int i=0; i<10; i++)
		if (fork() == 0)
			break;

#define BUFSIZE 65535
	int connfd;
	char readbuf[BUFSIZE];

	for (;;)
	{
		connfd = accept(sockfd, NULL, NULL);
		for (int i=0; i<BUFSIZE; i++)
			readbuf[i] = '\0';

		for (unsigned long long i=0; i<BUFSIZE; i++)
		{
			char c;
			if (read(connfd, &c, sizeof(c)) == 0) break;	// EOF
			else readbuf[i] = c;

			if (i > 3 && readbuf[i]==readbuf[i-2] && readbuf[i]=='\n'
					&& readbuf[i-1]==readbuf[i-3]
				       	&& readbuf[i-1]=='\r')
				break;
		}

		if (strncmp(readbuf, "GET", 3) == 0)
		{
			write(connfd, reply_200, strlen(reply_200));
			write(connfd, terminate_headers, strlen(terminate_headers));

			int indexpage_fd;
			if ((indexpage_fd = open("index.html", O_RDONLY)) == -1)
				return perror("Could not open index.html"),
				       EXIT_FAILURE;
			char c;
			while (read(indexpage_fd, &c, sizeof(c)) != 0)
				write(connfd, &c, sizeof(c));

			close(indexpage_fd);
			close(connfd);
		}

		else if (strncmp(readbuf, "POWEROFF / ", 11) == 0)
		{
			write(connfd, reply_200, strlen(reply_200));
			write(connfd, terminate_headers, strlen(terminate_headers));
			close(connfd);
			if (execlp(EXEC_COMMAND, EXEC_COMMAND, NULL) == -1)
				perror("Could not exec");
		}
#ifdef EBUG
		for (int i=0; i<=strlen(readbuf); i++)
			if (readbuf[i] == '\r') fputs("\\r", stderr);
			else if (readbuf[i] == '\n') fputs("\\n\n", stderr);
			else fprintf(stderr, "%c", readbuf[i]);
#endif /* EBUG */
	}
}

/* vim:set noet sw=8 sts=8 ts=8 ft=c fdm=syntax: */
