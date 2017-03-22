/* 
   Dylan Brown
   Lab 2
   CSE 2431

   UDP sender code.
 */


//sender = server

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "senderPackets.h"

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

int main(int argc, char *argv[])
{

 checkForCorrectNumberArguments(argc);
 /*
 * Declare initial variables
 */ 
 struct sockaddr_in serv_addr, client_addr;
 int sockfd, portno;
 socklen_t clilen;
 int probOfLoss = atof(argv[2]) * 100;
 portno = atoi(argv[1]);
 sockfd = socket(AF_INET, SOCK_DGRAM, 0); //create a new socket
 if (sockfd < 0){
    error("ERROR opening socket");
 }




  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
  serv_addr.sin_port = htons(portno); //convert from host to network byte order

  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){    
   error("ERROR on binding");
  } //Bind the socket to the server address


 char filename[MAXFILENAMESIZE];
 bzero(filename, MAXFILENAMESIZE);
 clilen = sizeof(client_addr);

 int recvlen = recvfrom(sockfd, filename, MAXFILENAMESIZE, 0, (struct sockaddr*)&client_addr, &clilen);
 if (recvlen < 0){
   error("ERROR receiving packet.\n");
 }
 printf("File name sent to the server is: %s\n", filename);

 sendFile(filename, sockfd, client_addr, clilen, probOfLoss);


 close(sockfd);
 return 0;
}
