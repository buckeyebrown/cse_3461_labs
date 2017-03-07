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

int main(int argc, char *argv[])
{
	 if (argc != 4) {
         fprintf(stderr,"ERROR, the receiver requires 4 arguments\n");
         exit(1);
     }

	int sockfd; //descriptors rturn from socket and accept system calls
	char* sender_hostname = argv[1];
    int portno = atoi(argv[2]); // port number 5434
    char* filename = argv[3];
    socklen_t clilen;
    char filebuffer[1024];
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t addrlen = sizeof(client_addr);

    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
    if (sockfd < 0) 
        error("ERROR opening socket");

    if(sendto(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, addrlen) < 0){
       error("ERROR on send to.\n");
    }


	return 0;
}