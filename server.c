/* Server side - UDP Code				    */
/* By Hugh Smith	4/1/2017	*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "gethostbyname.h"
#include "networks.h"
#include "safeUtil.h"
#include "handlePDU.h"

#define MAXBUF 80

void processClient(int socketNum);
int checkArgs(int argc, char *argv[]);

int main ( int argc, char *argv[]  )
{ 
	int socketNum = 0;				
	int portNumber = 0;

	portNumber = checkArgs(argc, argv);
		
	socketNum = udpServerSetup(portNumber);

	processClient(socketNum);

	close(socketNum);
	
	return 0;
}

void processClient(int socketNum)
{
	int dataLen = 0; 
	char buffer[MAXBUF + 1];
	uint8_t pduBuffer[MAXBUF + 1];	  
	struct sockaddr_in6 client;		
	int clientAddrLen = sizeof(client);	
	char pduBuffer[MAXBUF + 1];
	uint8_t flag = 1;

	uint32_t sequenceNumber = 0;
	buffer[0] = '\0';
	while (buffer[0] != '.')
	{
		dataLen = safeRecvfrom(socketNum, pduBuffer, MAXBUF, 0, (struct sockaddr *) &client, &clientAddrLen);
	
		printf("Received message from client with ");
		printIPInfo(&client);
		//printf(" Len: %d \'%s\'\n", dataLen, buffer);
		
		// call printPDU for testing
		printPDU(pduBuffer, dataLen);

		// just for fun send back to client number of bytes received
		sprintf(buffer, "bytes: %d", dataLen);
		
		int pduLen = createPDU(pduBuffer, sequenceNumber, flag, buffer, strlen(buffer) + 1);

		//increment sequence number
		sequenceNumber += 1;
		
		safeSendto(socketNum, buffer, pduLen, 0, (struct sockaddr *) & client, clientAddrLen);

	}
}

int checkArgs(int argc, char *argv[])
{
	// Checks args and returns port number
	int portNumber = 0;

	if (argc > 3 || argc < 2)
	{
		fprintf(stderr, "Usage: %s error-rate [optional port number]\n", argv[0]);
		exit(-1);
	}

	// check error rate
	float errorRate = atof(argv[1]);
	if((errorRate <= 0) || (errorRate >= 1)){
		printf("Error Rate value must be between 0 and 1\n");
		exit(-1)
	}

	// if optional port number is provided
	if (argc == 3)
	{
		portNumber = atoi(argv[2]);
	}
	
	return portNumber;
}


