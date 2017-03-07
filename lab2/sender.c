/* 
   Dylan Brown
   Lab 2
   CSE 2431

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

//1 KB for data
#define DATA 1024
//Header: Source Port # (2 bytes), Dest Port # (2 bytes), Length (2 bytes)
#define HEADER 6
//Header + Data
#define PACKET_SIZE 1030
//True
#define TRUE 1

int main(int argc, char *argv[])
{
	 if (argc != 2) {
         fprintf(stderr,"ERROR, the sender requires 2 arguments\n");
         exit(1);
     }

	  int sockfd; //descriptors rturn from socket and accept system calls
    int portno = atoi(argv[1]);



    socklen_t clilen;
    char filebuffer[PACKET_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server; //contains tons of information, including the server's IP address
    socklen_t addrlen = sizeof(client_addr);
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //initialize server's address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);


    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
    if (sockfd < 0) 
        error("ERROR opening socket");

    if(sendto(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, addrlen) < 0){
       error("ERROR on send to.\n");
    }


	return 0;
}