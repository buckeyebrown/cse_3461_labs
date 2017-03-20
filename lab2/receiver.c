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

//1 KB for data
#define DATA 1024
//Header: Sequence Number: 1 bytes ; Last Seq Number: 1 bytes
#define HEADER 6
//Header + Data
#define PACKET_SIZE 1030
//True
#define TRUE 1

void error(char* msg);
char* concat(const char *string_1, const char *string_2);
void readHeaderAndData(char* packetBuffer, char* dataBuffer, int* sequenceNumber, int* maxSequenceNumber, int* datasize);

int main(int argc, char *argv[])
{
	 if (argc != 4) {
         error("ERROR, the receiver requires 4 arguments\n");
     }
    int sockfd, portno;
    char* sender_hostname;
    socklen_t addrlen;
    struct sockaddr_in serv_addr;
    struct hostent *server; //contains tons of information, including the server's IP address
    char filebuffer[PACKET_SIZE];

    sender_hostname = argv[1];
    portno = atoi(argv[2]);
    char filename[64];
    bzero(filename, 64);
    memcpy(filename, argv[3], 64);

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

    printf("\n%lu\n!\n", strlen(filename));

    if(sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr*)&serv_addr, addrlen) < 0){
     	error("ERROR on send to. First time.\n");
    }    

    int n = 0;


    char packetBuffer[PACKET_SIZE];
    char dataBuffer[DATA];
    //while waiting for a response
    //while(n == 0){
    	recvlen = recvfrom(sockfd, packetBuffer, PACKET_SIZE, 0, (struct sockaddr*)&serv_addr, &addrlen);
    	printf("Received %d bytes.\n",recvlen);
    	if (recvlen < 0){
    	 	error("ERROR receiving packet.\n");
    	}
    	else if (recvlen > 0){
    	//	n = atoi(filebuffer);
    	//	printf("%d\n",n);
    		int sequenceNumber, maxSequenceNumber, datasize;
    		readHeaderAndData(packetBuffer, dataBuffer, &sequenceNumber, &maxSequenceNumber, &datasize);

    	}
    //}

    char* newfilename = concat("transferred_", filename); //concat string, ie, transferred_image.jpg
    FILE* filepointer = fopen(newfilename, "wb");
    free(newfilename); //free malloc from concat string
    fwrite(dataBuffer, 1, 15, filepointer);
    fclose(filepointer);
    bzero(packetBuffer, PACKET_SIZE);
    bzero(dataBuffer, DATA);
    
    /**
    while(TRUE){

     	 //if(sendto(sockfd, filename, strlen(filename), 0, (struct sockaddr*)&serv_addr, addrlen) < 0){
    	 //	error("ERROR on send to.\n");
    	 //}

    	 recvlen = recvfrom(sockfd, filebuffer, 15, 0, (struct sockaddr*)&serv_addr, &addrlen);
    	 printf("Received %d bytes.\n",recvlen);
    	 if (recvlen < 0){
    	 	error("ERROR receiving packet.\n");
    	 }
    	 //sendFile(filename, sockfd);
    }
    */
    close(sockfd);
	return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
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

void readHeaderAndData(char* packetBuffer, char* dataBuffer, int* sequenceNumber, int* maxSequenceNumber, int* datasize){
	char seqNumArr[1];
	char maxSeqNumArr[1];
	char dataSizeArry[4];

	int offset = 0;

	memcpy(seqNumArr, packetBuffer, 1);
	*sequenceNumber = atoi(seqNumArr);
	offset += 1;
	printf("\nThe Sequence number: %d", *sequenceNumber);

	memcpy(maxSeqNumArr, packetBuffer + offset, 1);
	*sequenceNumber = atoi(maxSeqNumArr);
	offset += 1;
		printf("\nThe Max Sequence number: %d", *maxSequenceNumber);


	memcpy(dataSizeArry, packetBuffer + offset, 4);
	*datasize = atoi(dataSizeArry);
	offset += 4;
	printf("\nThe Data Size: %d\n", *datasize);

	memcpy(dataBuffer, packetBuffer + offset, *datasize);


}