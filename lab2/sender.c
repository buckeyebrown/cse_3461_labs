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

//1 KB for data
#define DATA 1024
//Header: Source Port # (2 bytes), Dest Port # (2 bytes), Length (2 bytes)
#define HEADER 6
//Header + Data
#define PACKET_SIZE 1030
//True
#define TRUE 1
//False
#define FALSE 0

void error(char* msg);
void sendFile(char* filename, int sockfd, struct sockaddr_in client_addr, socklen_t clilen);

int main(int argc, char *argv[])
{
	 if (argc != 2) {
         fprintf(stderr,"ERROR, the sender requires 2 arguments\n");
         exit(1);
     }

     /*sockaddr_in: Structure Containing an Internet Address*/
     /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, client_addr;
    int sockfd, portno;
    socklen_t clilen;
    char filebuffer[PACKET_SIZE];


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
    clilen = sizeof(client_addr);

    recvlen = recvfrom(sockfd, filename, 64, 0, (struct sockaddr*)&client_addr, &clilen);
    printf("Received %d bytes.\n",recvlen);
    if (recvlen < 0){
       error("ERROR receiving packet.\n");
    }

    printf("File name sent to the server is: %s\n", filename);

    sendFile(filename, sockfd, client_addr, clilen);

    printf("here\n");
    while(TRUE){

      //recvlen = recvfrom(sockfd, filebuffer, 1024, 0, (struct sockaddr*)&client_addr, &clilen);
      //printf("Received %d bytes.\n",recvlen);
      if (recvlen < 0){
       error("ERROR receiving packet.\n");
      }

      //char filename[64];
      //memcpy(filename, buf, recvlen);

      if(sendto(sockfd, filebuffer, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen) < 0){
        error("ERROR on send to.\n");
      }

      bzero(filebuffer,PACKET_SIZE);

      int n = read(sockfd,filebuffer,PACKET_SIZE); //Read is a block function. It will read at most 255 bytes
       if (n < 0) error("ERROR reading from socket");
    }
  close(sockfd);
	return 0;
}

void error(char *msg)
{
    perror(msg);
    exit(1);
}

void sendFile(char* filename, int sockfd, struct sockaddr_in client_addr, socklen_t clilen){

    long fsize;
    FILE *filepointer = fopen(filename, "rb"); //Open file stream for the text.html file

    if (!filepointer){
      perror("The file cannot be opened.");
      exit(1);
    }

    fseek(filepointer, 0, SEEK_END); //Move "cursor" to the end of the file
    fsize = ftell(filepointer); //Get the position of the cursor as the length of the file

    if (fsize == -1){
      perror("The file size cannot be retrieved.");
      exit(1);
    }

    rewind(filepointer); //Move cursor back to start

    char *file_data = (char*) malloc(fsize); //Allocate the memory for the size of the file_data

    if (!file_data){
      perror("The file buffer could not be allocated in memory.");
      exit(1);
    }

    int cond = TRUE;
    while(cond){
      int n = fread(file_data, PACKET_SIZE, 1, filepointer);
      if (n > 0){
        if(sendto(sockfd, file_data, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen) < 0){
          error("ERROR on send to.\n");
        }
        //write(sockfd,file_data,PACKET_SIZE);
        }
      else{
        if(sendto(sockfd, file_data, PACKET_SIZE, 0, (struct sockaddr*)&client_addr, clilen) < 0){
          error("ERROR on send to.\n");
        }
        //write(sockfd,file_data,PACKET_SIZE);
        if (feof(filepointer)){
          printf("End of file.\n");
          cond = FALSE;
        }
        if (ferror(filepointer)){
          printf("Error reading this file.\n");
          cond = FALSE;
        }
      }
    }

    fclose(filepointer); //close the IO stream for the file 
    free(file_data);
}