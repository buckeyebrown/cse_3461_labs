#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "receiverPackets.h"

//1 KB for data
#define DATA 1024
//Header: Sequence Number: 1 bytes ; Last Seq Number: 1 bytes
#define HEADER 7
//Header + Data
#define PACKET_SIZE 1031
//Max Filename Size
#define MAXFILENAMESIZE 64
//True
#define TRUE 1
//False
#define FALSE 0
//Data Header Type
#define DATA_TYPE 1
//ACK Header Type
#define ACK_TYPE 2
//Timeout
#define TIMEOUT 1


void error(char *msg)
{
  perror(msg);
  exit(1);
}

int checkIfSequenceIsDuplicate(int sequenceNumber, int* previousSequenceNumbers, int maxSequenceNumber){
    //Return 1 if it is unique, return 0 if it is a duplicate.
    int j = 0;

    while (j < maxSequenceNumber){
        if(previousSequenceNumbers[j] == sequenceNumber + 1){
            printf("Duplicate packet. Packet ignored, resending ACK now.\n\n");
            return 0;
        }
        j++;
    }
    return 1;
}

char* concat(const char *string_1, const char *string_2)
{
	char *result_string = malloc(strlen(string_1)+strlen(string_2)+1);
	if (result_string == NULL){
		error("String concat failed at memory allocation.\n");
	}
	strcpy(result_string, string_1);
	strcat(result_string, string_2);
	return result_string;
}

int determineIfPacketWasDropped(int probOfLoss){
	// returns 1 if it was not dropped
  int ret, randomVal;
  randomVal = rand() % 100;

  ret = randomVal > probOfLoss;
  return ret;
}




int readHeaderAndData(char* packetBuffer, char* dataBuffer, int* packetType, int* maxSequenceNumber, int* datasize){
  char headerBuffer[HEADER];
  bzero(headerBuffer, HEADER);
  memcpy(headerBuffer, packetBuffer, HEADER);

  char packetTypeArr[1];
  bzero(packetTypeArr, 1);
  char seqNumArr[1];
  bzero(seqNumArr, 1);
  char maxSeqNumArr[1];
  bzero(maxSeqNumArr, 1);
  char dataSizeArry[4];
  bzero(dataSizeArry, 4);

	int offset = 0;

	memcpy(packetTypeArr, headerBuffer + offset, 1);
	*packetType = atoi(packetTypeArr);
	offset += 1;

	memcpy(seqNumArr, headerBuffer + offset, 1);
	int sequenceNumber = atoi(seqNumArr);
	offset += 1;

	memcpy(maxSeqNumArr, headerBuffer + offset, 1);
	*maxSequenceNumber = atoi(maxSeqNumArr);
	offset += 1;

	memcpy(dataSizeArry, headerBuffer + offset, 4);
	*datasize = atoi(dataSizeArry);
	offset += 4;
	int packet_type = *packetType;

	if (packet_type == DATA_TYPE){  
		printf("Reading header. This is a Data header, sequence number %d of %d, with the data size of %d.\n\n", sequenceNumber, *maxSequenceNumber, *datasize);  		
		memcpy(dataBuffer, packetBuffer + HEADER, *datasize);
	}
	return sequenceNumber;
}

void sendAck(int sequenceNumber, int *maxSequenceNumber, struct sockaddr_in serv_addr, socklen_t addrlen, int sockfd){
	char headerBuffer[HEADER];
	bzero(headerBuffer, HEADER);

	int max = *maxSequenceNumber;
	
	sprintf(headerBuffer, "%d%d%d%04d", ACK_TYPE, sequenceNumber, max, 0);
	
	if(sendto(sockfd, headerBuffer, strlen(headerBuffer), 0, (struct sockaddr*)&serv_addr, addrlen)<0){
     	error("ERROR on send to.\n");
	}
}