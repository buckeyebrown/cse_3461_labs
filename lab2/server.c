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

int main(int argc, char *argv[])
{
	 if (argc != 4) {
         fprintf(stderr,"ERROR, the receiver requires 4 arguments\n");
         exit(1);
     }

	int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
	char* sender_hostname = argv[1];
    int portno = atoi(argv[2]); // port number 5434
    char* filename = argv[3];
    socklen_t clilen;


	return 0;
}