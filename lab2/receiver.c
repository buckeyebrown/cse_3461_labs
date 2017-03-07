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
void sendFile(char* filename, int sockfd);

int main(int argc, char *argv[])
{
	 if (argc != 4) {
         error("ERROR, the receiver requires 4 arguments\n");
     }
    int sockfd, portno;
    char* sender_hostname, filename;
    socklen_t addrlen;
    struct sockaddr_in serv_addr;
    struct hostent *server; //contains tons of information, including the server's IP address
    char filebuffer[PACKET_SIZE];

    sender_hostname = argv[1];
    portno = atoi(argv[2]);
    filename = argv[3];

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

    while(TRUE){
     	 if(sendto(sockfd, filebuffer, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, addrlen) < 0){
    	 	error("ERROR on send to.\n");
    	 }

    	 recvlen = recvfrom(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, &addrlen);
    	 printf("Received %d bytes.\n",recvlen);
    	 if (recvlen < 0){
    	 	error("ERROR receiving packet.\n");
    	 }

    }

	return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void sendFile(char* filename, int sockfd){

	  long fsize;
	  FILE *filepointer = fopen(filename, "rb"); //Open file stream for the text.html file

	  if (!filepointer){
	  	perror("The image file cannot be opened.");
	  	exit(1);
	  }

	  fseek(filepointer, 0, SEEK_END); //Move "cursor" to the end of the file
	  fsize = ftell(filepointer); //Get the position of the cursor as the length of the file

	  if (fsize == -1){
	  	perror("The image file size cannot be retrieved.");
	  	exit(1);
	  }

	  rewind(filepointer); //Move cursor back to start

	  char *file_data = (char*) malloc(fsize); //Allocate the memory for the size of the file_data

	  if (!file_data){
	  	perror("The file buffer could not be allocated in memory.");
	  	exit(1);
	  }

	  while(TRUE){
		  int n = fread(file_data, PACKET_SIZE, 1, filepointer);
		  if (n > 0){
		  	write(newsockfd,file_data,PACKET_SIZE);
		  	}
		  else{
		  	write(newsockfd,file_data,PACKET_SIZE);
		  	if (feof(filepointer)){
		  		printf("End of file.\n");
		  		j = 0;
		  	}
		  	if (ferror(filepointer)){
		  		printf("Error reading this file.\n");
		  		j = 0;
		  	}
		  }
	  }

	  fclose(filepointer); //close the IO stream for the file 
	  free(file_data);
}