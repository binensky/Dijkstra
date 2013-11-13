#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include <sys/types.h>
#include <sys/stat.h>

#define RCVBUFSIZE 32   /* Size of receive buffer */
#define BUFSIZE	256 

int main(int argc, char *argv[]) {
	int sock;                        /* Socket descriptor */
	struct sockaddr_in echoServAddr; /* Echo server address */
	unsigned short echoServPort;     /* Echo server port */
	char *servIP;                    /* Server IP address (dotted quad) */
	int bytesRcvd = 0, totalBytesRcvd = 0;   /* Bytes read in single recv() and total bytes read */

	char fileBuf[BUFSIZE];

	char buffer[1024];

	servIP = argv[1];             /* First arg: server IP address (dotted quad) */
	//echoString = argv[2];         /* Second arg: string to echo */

	echoServPort = 10000;

	/* Create a reliable, stream socket using TCP */
	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
	{
		printf("socket() failed\n");
		return 0;
	}
		//DieWithError("socket() failed");
	/* Construct the server address structure */
	memset(&echoServAddr, 0, sizeof(echoServAddr));     /* Zero out structure */
	echoServAddr.sin_family      = AF_INET;             /* Internet address family */
	echoServAddr.sin_addr.s_addr = inet_addr(servIP);   /* Server IP address */
	echoServAddr.sin_port        = htons(echoServPort); /* Server port */

	/* Establish the connection to the echo server */
	if (connect(sock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		printf("connect() failed\n");
	else{
		printf("connect success!!\n");
		scanf("%s", buffer);

		memset(buffer, 0, sizeof(buffer));

		while(1)
		{
			if ((bytesRcvd = recv(sock, buffer, sizeof(buffer), 0)) < 0)
				printf("recv() failed");
			buffer[bytesRcvd] = 0;
			printf("%s\n", buffer);
		}
		close(sock);
		exit(0);
	}
}
