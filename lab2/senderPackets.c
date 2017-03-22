#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "packets.h"

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

void sendFile(char* filename, int sockfd, struct sockaddr_in client_addr, socklen_t clilen, int probOfLoss){

  long fsize;
    FILE *filepointer = fopen(filename, "rb"); //Open file stream for the text.html file


    if (!filepointer){
      error("The file cannot be opened.");
      exit(1);
    }

    fseek(filepointer, 0, SEEK_END); //Move "cursor" to the end of the file
    fsize = ftell(filepointer); //Get the position of the cursor as the length of the file

    if (fsize == -1){
      error("The file size cannot be retrieved.");
      exit(1);
    }
    printf("\nThe fsize is: %lu\n", fsize);

    int maxSeqNum = ((int) fsize / DATA);

    printf("The max seq num is: %d\n", maxSeqNum);
    if (maxSeqNum > 9){
      error("File too large.\n");
    }
    rewind(filepointer); //Move cursor back to start

    //char *file_data = (char*) malloc(fsize); //Allocate the memory for the size of the file_data
    srand(time(NULL));
    int sequenceNumber = 0;

    char packetBuffer[PACKET_SIZE];
    bzero(packetBuffer, PACKET_SIZE);

    while (sequenceNumber < maxSeqNum + 1){
      makePacket(packetBuffer, DATA_TYPE, sequenceNumber, filepointer, maxSeqNum);

      if (determineIfPacketWasDropped(probOfLoss)){
      	printf("\n Sending packet %d of %d to receiver.\n", sequenceNumber, maxSeqNum);
        if(sendto(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen)<0){
         error("ERROR on send to.\n");
       }
       int q = FALSE;

        while(!q){
          q = waitForAck(sockfd,client_addr, clilen, packetBuffer, probOfLoss);
        }
    }
    else{
      printf("Error, packet %d of %d was lost.\n", sequenceNumber, maxSeqNum);
        int q = FALSE;
        while(!q){
          q = waitForAck(sockfd,client_addr, clilen, packetBuffer, probOfLoss);
        }
    }
    sequenceNumber++;
  }

}

void makePacket(char *file_data, int headerType, int sequenceNumber, FILE* filepointer, int maxSeqNum){
  int datasize = 0;

  //Read, but skip over the header bytes
  //make sure file_data is a buffer of size PACKET_SIZEPACKET_SIZE
  //figure a way to determine file position/
  int filePosition = sequenceNumber * DATA;
  //sequence number * 1024 bytes, starting at 0

  fseek(filepointer, filePosition, SEEK_SET);
  datasize = fread(file_data + HEADER, 1, DATA, filepointer);

  createDataHeader(file_data, DATA_TYPE, sequenceNumber, maxSeqNum, datasize);
}

void createDataHeader(char *filebuffer, int headerType, int sequenceNumber, int maxSequenceNumber, int filesize){
  //declare a header of size HEADER, 6 because each int = 2 bytes
  
  char headerBuf[HEADER];
  bzero(headerBuf, HEADER);
  sprintf(headerBuf, "%d%d%d%04d", headerType, sequenceNumber, maxSequenceNumber, filesize);
  memcpy(filebuffer, headerBuf, HEADER);

}

int determineIfPacketWasDropped(int probOfLoss){
	// returns 1 if it was not dropped
  int ret, randomVal;
  randomVal = rand() % 100;
  printf("\n%d is the random value. Versus %d.\n", randomVal, probOfLoss);

  ret = randomVal > probOfLoss;
  return ret;
}

int waitForAck(int sockfd, struct sockaddr_in client_addr, socklen_t clilen, char* packetBuffer, int probOfLoss){
  int recvlen, packetType, sequenceNumber, maxSequenceNumber, datasize;
  char ack[HEADER];

  struct timeval timeout={TIMEOUT,0}; 
  setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(struct timeval));
  int q = FALSE;
  while(!q){    
    recvlen = recvfrom(sockfd, ack, HEADER, 0, (struct sockaddr*)&client_addr, &clilen);
    if (recvlen < 0){
        if (determineIfPacketWasDropped(probOfLoss)){
          if(sendto(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen)<0){
           error("ERROR on send to.\n");
         }
          else{
            q = TRUE;
          }
        }
        else
        {
          printf("\nERROR, packet was dropped.\n");
        }
   }

   else{
    readHeaderAndACK(ack, &packetType, &sequenceNumber, &maxSequenceNumber, &datasize);
    if (packetType == ACK_TYPE){
      printf("ACK Successfully received. For packet %d out of %d.\n", sequenceNumber, maxSequenceNumber);
      return TRUE;
    }
  }
}

return FALSE;
}

void readHeaderAndACK(char* packetBuffer, int* packetType, int* sequenceNumber, int* maxSequenceNumber, int* datasize){
  char packetTypeArr[1];
  bzero(packetTypeArr, 1);
  char seqNumArr[1];
  bzero(seqNumArr, 1);
  char maxSeqNumArr[1];
  bzero(maxSeqNumArr, 1);
  char dataSizeArry[4];
  bzero(dataSizeArry, 4);


  int offset = 0;

  memcpy(packetTypeArr, packetBuffer + offset, 1);
  *packetType = atoi(packetTypeArr);
  offset += 1;

  memcpy(seqNumArr, packetBuffer + offset, 1);
  *sequenceNumber = atoi(seqNumArr);
  offset += 1;

  memcpy(maxSeqNumArr, packetBuffer + offset, 1);
  *maxSequenceNumber = atoi(maxSeqNumArr);
  offset += 1;


  memcpy(dataSizeArry, packetBuffer + offset, 4);
  *datasize = atoi(dataSizeArry);
  offset += 4;
}