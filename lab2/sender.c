/* 
   Dylan Brown
   Lab 2
   CSE 2431

   UDP client code.
 */


//sender = server

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

//1 KB for data
#define DATA 1024
//Header: Sequence Number: 1 bytes ; Last Seq Number: 1 bytes
#define HEADER 6
//Header + Data
#define PACKET_SIZE 1030
//True
#define TRUE 1
//False
#define FALSE 0

void error(char* msg);
void sendFile(char* filename, int sockfd, struct sockaddr_in client_addr, socklen_t clilen, int probOfLoss);
void createDataHeader(char* filebuffer, int sequenceNumber, int maxSequenceNumber, int filesize);
void makePacket(char* file_data, int sequenceNumber, FILE* filepointer, int maxSeqNum);


int main(int argc, char *argv[])
{
  if (argc != 3) {
   fprintf(stderr,"ERROR, the sender requires 2 arguments.\n");
   exit(1);
 }

     /*sockaddr_in: Structure Containing an Internet Address*/
 struct sockaddr_in serv_addr, client_addr;
 int sockfd, portno;
 socklen_t clilen;
 int probOfLoss = atof(argv[2]) * 100;

 portno = atoi(argv[1]);
  sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create a new socket
  if (sockfd < 0) 
    error("ERROR opening socket");


  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
  serv_addr.sin_port = htons(portno); //convert from host to network byte order

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
   error("ERROR on binding");

 int recvlen;

 char filename[64];
 bzero(filename, 64);
 clilen = sizeof(client_addr);

 recvlen = recvfrom(sockfd, filename, 64, 0, (struct sockaddr*)&client_addr, &clilen);
 printf("Received %d bytes.\n",recvlen);
 if (recvlen < 0){
   error("ERROR receiving packet.\n");
 }

 printf("File name sent to the server is: %s\n", filename);

 sendFile(filename, sockfd, client_addr, clilen, probOfLoss);

 close(sockfd);
 return 0;
}

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
    while (sequenceNumber < maxSeqNum + 1){
      makePacket(packetBuffer, sequenceNumber, filepointer, maxSeqNum);
      int randomVal = rand() % 100;
      printf("\n%d is the random value. Versus %d.\n", randomVal, probOfLoss);
      if (randomVal > probOfLoss){
        if(sendto(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen)<0){
         error("ERROR on send to.\n");
       }
     }
     else{
      printf("Error, packet %d of %d was lost.\n", sequenceNumber, maxSeqNum);
     }
     sequenceNumber++;
   }

 }

 void createDataHeader(char *filebuffer, int sequenceNumber, int maxSequenceNumber, int filesize){
  //declare a header of size HEADER, 6 because each int = 2 bytes
  char headerBuf[HEADER];
  sprintf(headerBuf, "%d%d%04d", sequenceNumber, maxSequenceNumber, filesize);
  memcpy(filebuffer, headerBuf, HEADER);
  //printf("\nThe sequence number is: %d\n", sequenceNumber);
  //printf("\nThe MAX sequence number is: %d\n", maxSequenceNumber);
  //printf("\nThe file size is: %d\n", filesize);

}

void makePacket(char *file_data, int sequenceNumber, FILE* filepointer, int maxSeqNum){
  int datasize = 0;

  //Read, but skip over the header bytes
  //make sure file_data is a buffer of size PACKET_SIZE
  //figure a way to determine file position/
  int filePosition = sequenceNumber * DATA;
  //sequence number * 1024 bytes, starting at 0

  fseek(filepointer, filePosition, SEEK_SET);
  datasize = fread(file_data + HEADER, 1, DATA, filepointer);

  createDataHeader(file_data, sequenceNumber, maxSeqNum, datasize);
}