#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <pwd.h>
#include <readpassphrase.h>

#include "headerT.h"

// the port client will be connecting to
#define PORT 3490

// max number of bytes we can get at once
#define MAXDATASIZE 300

extern char buf[MAX_SIZE];

void print_auswahlmenu()
{
	printf("\tTor 1, 2 oder 3?\n");
	printf("1: neuen Studenten hinzufügen\n");
	printf("2: Studentendaten zu einer ID empfangen\n");
	printf("3: die besten Studenten empfangen\n");
	printf("0: beenden\n");
	printf("> ");
}

void neuen_studenten_anlegen(int socket)
{
	printf("Name: "); fgets(buf, MAX_NAME, stdin);
}

void send_test(int socket)
{
	char s[] = "test\n";
	bzero(buf, MAX_SIZE);
	printf("sende \"test\\n\"\n");
	if (send(socket, s, strlen(s), 0) < 0) {
		perror("send_test error lol!");
	}
	if (recv(socket, buf, MAX_SIZE, 0) > 0) {
		printf("%s\n", buf);
	} else {
		printf("Nix empfangen.\n");
	}
	return;
}

int main(int argc, char *argv[])
{
	int sockfd, numbytes;
	char buf[MAXDATASIZE];
	struct hostent *he;

	// connectors address information:
	struct sockaddr_in their_addr;

	// if no command line argument supplied:
	if(argc != 2) {
		fprintf(stderr,
				"Client-Usage: %s the_client_hostname\n",
				argv[0]);
		exit(EXIT_FAILURE);
	}

	// get the host info
	if((he=gethostbyname(argv[1])) == NULL) {
		perror("gethostbyname()");
		exit(EXIT_FAILURE);
	}
	printf("Client-The remote host is: %s\n", argv[1]);
	
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket()");
		exit(EXIT_FAILURE);
	}
	printf("Client-The socket() sockfd is %d\n", sockfd);

	// short, network byte order:
	printf("Server-Using %s and port %d...\n", argv[1], PORT);

	// host byte order
	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(PORT);
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);

	// zero the rest of the struct
	memset(&(their_addr.sin_zero), '\0', 8);

	if(connect(sockfd,
				(struct sockaddr *) &their_addr,
				sizeof(struct sockaddr)
			) == -1) {
		perror("connect()");
		exit(EXIT_FAILURE);
	}
	printf("Client-The connect() is OK...\n");

	// Zauberwort senden:
	char alohomora[] = "auth\n";
	if ((numbytes = send(sockfd, alohomora, strlen(alohomora), 0)) < 0){
		perror("sending \"auth\\n\" failed");
		exit(EXIT_FAILURE);
	}

	// Benutzernamen einlesen:
	char * username[MAX_USERNAME];
	printf("Benutzername: ");
	fgets(username, MAX_USERNAME, stdin);

	// Benutzernamen senden:
	send(sockfd, username, strlen(username), 0);

	// Passwort einlesen:
	char * password = (char *) malloc(_PASSWORD_LEN);
	readpassphrase("Passwort: ", password, sizeof(password), 0);

	// Passwort senden
	// OMG - IM KLARTEXT!!!! Dafür stirbt gerade eine Nyan Katze.
	send(sockfd, password, strlen(password), 0);

	// Antwort von Authentifizierung empfangen:
	if((numbytes = recv(sockfd, buf, MAX_SIZE, 0)) < 0) {
		perror("recv()");
		exit(EXIT_FAILURE);
	} else {
		printf("=> %s", buf);
	}

	char cmd = ' ';
	while (cmd != '0') {
		print_auswahlmenu();
		cmd = fgetc(stdin);
		switch (cmd) {
			case '0':
				break;
			case 't':
				send_test(sockfd);
				break;
		}
	}

	/*buf[numbytes] = '\0';*/

	printf("Client-Closing sockfd\n");
	close(sockfd);

	return EXIT_SUCCESS;
}
