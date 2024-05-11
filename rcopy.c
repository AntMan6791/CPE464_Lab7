// Client side - UDP Code				    
// By Hugh Smith	4/1/2017		

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "handlePDU.h"

#define MAXBUF 80

void talkToServer(int socketNum, struct sockaddr_in6 * server);
int readFromStdin(char * buffer);
int checkArgs(int argc, char * argv[]);


int main (int argc, char *argv[])
 {
	int socketNum = 0;				
	struct sockaddr_in6 server;		// Supports 4 and 6 but requires IPv6 struct
	int portNumber = 0;
	
	portNumber = checkArgs(argc, argv);
	
	socketNum = setupUdpClientToServer(&server, argv[2], portNumber);
	
	talkToServer(socketNum, &server);
	
	close(socketNum);

	return 0;
}

void talkToServer(int socketNum, struct sockaddr_in6 * server)
{
	int serverAddrLen = sizeof(struct sockaddr_in6);
	char * ipString = NULL;
	int dataLen = 0;
	int pduLen = 0;
	char inBuffer[MAXBUF+1];
	uint8_t pduBuffer[MAXBUF+1];
	uint8_t flag = 1;

	uint32_t sequenceNumber = 0;
	inBuffer[0] = '\0';
	while (inBuffer[0] != '.')
	{
		dataLen = readFromStdin(inBuffer);

		printf("Sending: %s with len: %d\n", inBuffer,dataLen);
	
		// create PDU to send
		pduLen = createPDU(pduBuffer, sequenceNumber, flag, (uint8_t *) inBuffer, dataLen);

		//increment sequence number
		sequenceNumber += 1;

		safeSendto(socketNum, pduBuffer, pduLen, 0, (struct sockaddr *) server, serverAddrLen);
		
		// after this call, pduBuffer will have the server's pdu
		int recvLen = safeRecvfrom(socketNum, pduBuffer, MAXBUF, 0, (struct sockaddr *) server, &serverAddrLen);
		
		// print out bytes received
		ipString = ipAddressToString(server);
		printf("Server with ip: %s and port %d said it received %s\n", ipString, ntohs(server->sin6_port), pduBuffer);
	    
		// call printPDU for testing
		printf("Call printPDU()\n\n");
		printPDU(pduBuffer, recvLen);
	}
}

int readFromStdin(char * buffer)
{
	char aChar = 0;
	int inputLen = 0;        
	
	// Important you don't input more characters than you have space 
	buffer[0] = '\0';
	printf("Enter data: ");
	while (inputLen < (MAXBUF - 1) && aChar != '\n')
	{
		aChar = getchar();
		if (aChar != '\n')
		{
			buffer[inputLen] = aChar;
			inputLen++;
		}
	}
	
	// Null terminate the string
	buffer[inputLen] = '\0';
	inputLen++;
	
	return inputLen;
}

int checkArgs(int argc, char * argv[])
{

        int portNumber = 0;
	
        /* check command line arguments  */
	if (argc != 4)
	{
		printf("usage: %s error-rate host-name port-number \n", argv[0]);
		exit(1);
	}

	// check error rate (assumes it is a number)
	float errorRate = atof(argv[1]);
	if((errorRate <= 0) || (errorRate >= 1)){
		printf("Error Rate value must be between 0 and 1\n");
		exit(1);
	}
	
	portNumber = atoi(argv[3]);
		
	return portNumber;
}





