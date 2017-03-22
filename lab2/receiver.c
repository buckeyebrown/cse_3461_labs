/* 
   Dylan Brown
   Lab 2
   CSE 2431

   UDP receiver code.
 */

//receiver == client

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <netdb.h>      // define structures like hostent
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>
#include "packets.h"

//1 KB for data
#define DATA 1024
//Header: Sequence Number: 1 bytes ; Last Seq Number: 1 bytes
#define HEADER 7
//Header + Data
#define PACKET_SIZE 1031
//True
#define TRUE 1
//False
#define FALSE 0
//Data Header Type
#define DATA_TYPE 1
//ACK Header Type
#define ACK_TYPE 2

int checkIfSequenceIsDuplicate(int sequenceNumber, int* previousSequenceNumbers, int maxSequenceNumber);

int main(int argc, char *argv[])
{
	if (argc != 5) {
		error("ERROR, the receiver requires 4 arguments\n");
	}
	int sockfd, portno;
	char* sender_hostname;
	socklen_t addrlen;
	struct sockaddr_in serv_addr;
    struct hostent *server; //contains tons of information, including the server's IP address

    sender_hostname = argv[1];
    portno = atoi(argv[2]);
    char filename[64];
    bzero(filename, 64);
    memcpy(filename, argv[3], 64);
    int probOfLoss = atof(argv[4]) * 100;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) 
    	error("ERROR opening socket");

    server = gethostbyname(sender_hostname); //takes a string like "www.yahoo.com", and returns a struct hostent which contains information, as IP address, address type, the length of the addresses...
    if (server == NULL) {
    	fprintf(stderr,"ERROR, no such host\n");
    	exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //initialize server's address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    int recvlen;
    addrlen = sizeof(serv_addr);

    //Send the filename to the client

    if(sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr*)&serv_addr, addrlen) < 0){
    	error("ERROR on send to. First time.\n");
    }    


    char packetBuffer[PACKET_SIZE];
    char dataBuffer[DATA];
    int packetType, sequenceNumber, maxSequenceNumber, datasize;
    int packetsReceived = 0;
    srand(time(NULL));

    int firstPacketReceivedAndACKed = FALSE;
    while(!firstPacketReceivedAndACKed){
        recvlen = recvfrom(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&serv_addr, &addrlen);
        if (recvlen < 0){
            error("ERROR receiving packet.\n");
        }
        else if (recvlen > 0){
        	sequenceNumber = readHeaderAndData(packetBuffer, dataBuffer, &packetType, &maxSequenceNumber, &datasize);
            //create an array of previousSequenceNumbers of size maxSequenceNumber. Zero it out.
            if (determineIfPacketWasDropped(probOfLoss)){
                sendAck(sequenceNumber, &maxSequenceNumber, serv_addr, addrlen, sockfd);
                packetsReceived++;
                firstPacketReceivedAndACKed = TRUE;
            }
            else{
                printf("ACK from Receiver to Sender failed.\n");
            }

        }
        
    }
    int previousSequenceNumbers[maxSequenceNumber];
    bzero(previousSequenceNumbers, maxSequenceNumber);
    previousSequenceNumbers[sequenceNumber] = sequenceNumber + 1;

    char* newfilename = concat("transferred_", filename); //concat string, ie, transferred_image.jpg
    FILE* filepointer = fopen(newfilename, "wb");
    free(newfilename); //free malloc from concat string
    fwrite(dataBuffer, 1, datasize, filepointer);
    int j = TRUE;
        int lastACKSent = FALSE;
    while(j && !lastACKSent){
        recvlen = recvfrom(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&serv_addr, &addrlen);
        printf("Received %d bytes.\n",recvlen);
        if (recvlen < 0){
        }
        else if (recvlen > 0){
          sequenceNumber = readHeaderAndData(packetBuffer, dataBuffer, &packetType, &maxSequenceNumber, &datasize);
            //Check if the sequence number is a duplicate
          if (checkIfSequenceIsDuplicate(sequenceNumber, previousSequenceNumbers, maxSequenceNumber)){
            previousSequenceNumbers[sequenceNumber] = sequenceNumber + 1;

            if (determineIfPacketWasDropped(probOfLoss)){
                sendAck(sequenceNumber, &maxSequenceNumber, serv_addr, addrlen, sockfd);                
            }
            else{
                printf("ACK from Receiver to Sender failed.\n");
            }

            if (packetType == DATA_TYPE){
               packetsReceived++;
               fwrite(dataBuffer, 1, datasize, filepointer);
           }

       }
       else{
        if (determineIfPacketWasDropped(probOfLoss)){
          sendAck(sequenceNumber, &maxSequenceNumber, serv_addr, addrlen, sockfd);     
          if (sequenceNumber == maxSequenceNumber){
            lastACKSent = TRUE;
        }           
    }
    else{
        printf("ACK from Receiver to Sender failed.\n");
        if (sequenceNumber == maxSequenceNumber){
            while(!lastACKSent){
                if (determineIfPacketWasDropped(probOfLoss)){
                    sendAck(sequenceNumber, &maxSequenceNumber, serv_addr, addrlen, sockfd);
                    lastACKSent = TRUE;                
                }
                else{
                    printf("ACK from Receiver to Sender failed.\n");
                }
            }
        }
    }
}
}
    	//fwrite(dataBuffer, 1, datasize, filepointer);
if((sequenceNumber == maxSequenceNumber)){
  j = FALSE;
}
}

if (packetsReceived == maxSequenceNumber + 1){
 printf("Received %d out of %d packets. Success.\n", packetsReceived, maxSequenceNumber+1);
}
else{
 printf("Received %d out of %d packets. Error.\n", packetsReceived, maxSequenceNumber+1);	
}
fclose(filepointer);
bzero(packetBuffer, PACKET_SIZE);
bzero(dataBuffer, DATA);

close(sockfd);
return 0;
}

int checkIfSequenceIsDuplicate(int sequenceNumber, int* previousSequenceNumbers, int maxSequenceNumber){
    //Return 1 if it is unique, return 0 if it is a duplicate.
    int j = 0;

    while (j < maxSequenceNumber){
        if(previousSequenceNumbers[j] == sequenceNumber + 1){
            printf("Duplicate packet. Ignored, then resending ACK.\n");
            return 0;
        }
        j++;
    }
    return 1;
}