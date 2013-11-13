#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

struct sockaddr_in echoServAddr;
struct sockaddr_in echoClntAddr;

int servSock;
int clntSock;
unsigned short echoServPort = 10000;
unsigned int clntLen;
int recvMsgSize;
char buffer[256];
char echoBuffer[256];        /* Buffer for echo string */
int recvFileSize;

int create_sock(){
	if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		printf("socket error");
	//Construct local address structure
	memset(&echoServAddr, 0, sizeof(echoServAddr)); 
	echoServAddr.sin_family = AF_INET;  
	echoServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	echoServAddr.sin_port = htons(echoServPort); 

	if (bind(servSock, (struct sockaddr *) &echoServAddr, sizeof(echoServAddr)) < 0)
		printf("bind error\n");
	if (listen(servSock, 5) < 0)
		printf("listen error\n");
	clntLen = sizeof(echoClntAddr);
	printf("Wait for connection!!\n");
	//Wait for a client to connect

	if ((clntSock = accept(servSock, (struct sockaddr *) &echoClntAddr, &clntLen)) < 0)
		printf("accept error\n");
	else
		printf("connect success!!\n");

	return clntSock;
}
