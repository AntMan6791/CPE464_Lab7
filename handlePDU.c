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
#include "checksum.h"

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int payloadLen);
void printPDU(uint8_t * aPDU, int pduLength);

int createPDU(uint8_t * pduBuffer, uint32_t sequenceNumber, uint8_t flag, uint8_t * payload, int payloadLen){
    int pduLength = payloadLen + 7;
    uint16_t checksum = 0;

    // Sequence number if net order
    uint32_t netSeqNum = htonl(sequenceNumber);
    
    // put data into the pduBuffer
    memcpy(pduBuffer, &netSeqNum, 4); // add sequence num
    memcpy(pduBuffer + 4, &checksum, 2);// reserve 2 bytes for checksu,
    memcpy(pduBuffer + 6, &flag, 1); // add flag
    memcpy(pduBuffer + 7, payload, payloadLen); // add payload
    
    // add checksum
    checksum = in_cksum((unsigned short*) pduBuffer, pduLength);
    memcpy(pduBuffer + 4, &checksum, 2);

    // return pduLength
    return pduLength;
}

void printPDU(uint8_t * aPDU, int pduLength){
    // verify checksum
    if(in_cksum((unsigned short*) aPDU, pduLength) != 0){
        printf("PDU was corrupted\n");
    }
/*    else{
        printf("PDU is intact\n");
    }*/
    
    // print out details
    uint32_t sequenceNumber;
    uint8_t flag;
    uint8_t payload[1400];
    int payloadLen = pduLength - 7;

    memcpy(&sequenceNumber, aPDU, 4);
    uint32_t hostSeqNum = ntohl(sequenceNumber);
    printf("Sequence Number: %d\n", hostSeqNum);

    memcpy(&flag, aPDU + 6, 1);
    printf("Flag: %d\n", flag);

    memcpy(payload, aPDU + 7, payloadLen);
    printf("Payload: %s\n", payload); // assume payload is string

    printf("Payload Length: %d\n\n", payloadLen);

}