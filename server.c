/* serverprog.c - a stream socket server demo */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include "headerT.h"

/* the port users will be connecting to */
#define MYPORT 3490
/* how many pending connections queue will hold */
#define BACKLOG 10

void sigchld_handler(int s)
{
	while(wait(NULL) > 0);
}

/* listen on sid, new connection on sid_neu*/
extern int sid;
extern int sid_neu;

// unser general purpose buffer: (general purpose *salutier*)
extern char buf[MAX_SIZE];

extern struct group g_info, g_itti, g_math;

int receive_command()
{
	// erstmal empfangen wir das "Kommando" vom socket:
	ssize_t n = recv(sid_neu, &buf, MAX_SIZE, 0);
	// und dann werten wir den Inhalt aus ...
	// ist überhaupt was angekommen?
	if (n < 0) {
		// falls nicht:
		return 1;
	} else { // es ist was angekommen
		// nur was?
		if (strcmp(buf, "test\n") == 0) {
			printf("test empfangen!\n");
			// zu Testzwecken geben wir ein paar Infos raus:
			char text[100];
			sprintf(text, "Hello Operating Systems student! This is PID %d on Socket %d talking.\n", getpid(), sid_neu);
			sending(text, strlen(text));
		} else if (strcmp(buf, "create\n") == 0) {
			receive_student();
		} else if (strcmp(buf, "auth\n") == 0) {
			auth_user();
			return 0;
		} else if (strcmp(buf, "\n") == 0 ) {
			return 0;
		}
	}
	return 0;
}

int main(int argc, char *argv[ ])
{
	/* my address information */
	struct sockaddr_in my_addr;
	/* connectors address information */
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	struct sigaction sa;
	int yes = 1;
	if ((sid = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server-socket() error lol!");
		exit(1);
	}
	else {
		printf("Server: socket() sid is OK\n");
	}
	if (setsockopt(
				sid,
				SOL_SOCKET,
				SO_REUSEADDR,
				&yes,
				sizeof(int)
			) == -1) {
		perror("Server-setsockopt() error lol!");
		exit(1);
	}
	else {
		printf("Server: setsockopt is OK\n");
	}
	/* host byte order */
	my_addr.sin_family = AF_INET;
	/* short, network byte order */
	my_addr.sin_port = htons(MYPORT);
	/* automatically fill with my IP */
	my_addr.sin_addr.s_addr = INADDR_ANY;
	printf("Server: Using %s and port %d...\n",
			inet_ntoa(my_addr.sin_addr), MYPORT);
	/* zero the rest of the struct */
	memset(&(my_addr.sin_zero), '\0', 8);
	if(bind(
				sid,
				(struct sockaddr *) &my_addr,
				sizeof(struct sockaddr)
			) == -1) {
		perror("Server: bind() error lol!");
		exit(1);
	}
	else {
		printf("Server: bind() is OK\n");
	}
	if(listen(sid, BACKLOG) == -1) {
		perror("Server: listen() error");
		exit(1);
	}
	printf("Server: listen() is OK\n");
	printf("Listening on socket %d\n", sid);
	/* clean all the dead processes */
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("Server-sigaction() error lol!");
		exit(1);
	}
	else {
		printf("Server: sigaction() is OK\n");
	}
	/* accept() loop */
	while(1) {
		sin_size = sizeof(struct sockaddr_in);
		if((sid_neu = accept(sid,
				(struct sockaddr *) &their_addr,
				&sin_size)) == -1) {
			perror("Server-accept() error lol!");
			continue;
		}
		else {
			printf("Server-accept() is OK...\n");
		}
		printf("Server: sid_neu: %d\n", sid_neu);
		printf("Server: Got connection from %s\n",
				inet_ntoa(their_addr.sin_addr));
		if(!fork()) {
			/* this is the child process */

			// child doesn't need the listener:
			close(sid);

			while (1) {
				if (receive_command()) {
					perror("Server-child: receive_command error lol!");
				}
			}

			close(sid_neu);
			exit(0);
		}
		else {
			/* this is the parent process */
			printf("Server: send is OK\n");
			/* parent doesn't need this */
			close(sid_neu);
			printf("Server: new socket, sid_neu closed\n");
		}
	}
	return 0;
}
