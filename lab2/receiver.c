/* 
   Dylan Brown
   Lab 2
   CSE 2431

 */

#include <stdio.h>
#include <sys/types.h>   // definitions of a number of data types used in socket.h and netinet/in.h
#include <sys/socket.h>  // definitions of structures needed for sockets, e.g. sockaddr
#include <netinet/in.h>  // constants and structures needed for internet domain addresses, e.g. sockaddr_in
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

//1 KB for data
#define DATA 1024
//Header: Source Port # (2 bytes), Dest Port # (2 bytes), Length (2 bytes)
#define HEADER 6
//Header + Data
#define PACKET_SIZE 1030
//True
#define TRUE 1

void error(char* msg);

int main(int argc, char *argv[])
{
	 if (argc != 4) {
         error("ERROR, the receiver requires 4 arguments\n");
     }

	char* sender_hostname = argv[1];
    int portno = atoi(argv[2]); // port number 5434
    char* filename = argv[3];
    int sockfd; //descriptors return from socket and accept system calls
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t addrlen = sizeof(client_addr);

    char filebuffer[1024];

    /*Create a new socket
    AF_INET: Address Domain is Internet 
    SOCK_STREAM: Socket Type is STREAM Socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");

	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //this code allows the socket to bind even though it's in TIME_WAIT

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");

    int recvlen;

    while(TRUE){
    	 recvlen = recvfrom(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, &addrlen);
    	 printf("Received %d bytes.\n",recvlen);
    	 if (recvlen < 0){
    	 	error("ERROR receiving packet.\n");
    	 }

    	 if(sendto(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, addrlen) < 0){
    	 	error("ERROR on send to.\n");
    	 }

    }

	return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}