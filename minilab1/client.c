
/*
 A simple client in the internet domain using TCP
 Usage: ./client hostname port (./client 192.168.0.151 10000)
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>      // define structures like hostent
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(0);
}

void sendFile(char* filename, int sockfd);


int main(int argc, char *argv[])
{
    int sockfd; //Socket descriptor
    int portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server; //contains tons of information, including the server's IP address

    char buffer[256];
    if (argc < 3) {
       fprintf(stderr,"usage %s hostname port\n", argv[0]);
       exit(0);
    }
    
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0); //create a new socket
    if (sockfd < 0) 
        error("ERROR opening socket");
    
    server = gethostbyname(argv[1]); //takes a string like "www.yahoo.com", and returns a struct hostent which contains information, as IP address, address type, the length of the addresses...
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; //initialize server's address
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) //establish a connection to the server
        error("ERROR connecting");
    
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    
    //n = send(sockfd,buffer,strlen(buffer),0); //send to the socket
    n = write(sockfd,buffer,strlen(buffer)); //write to the socket
    if (n < 0) 
         error("ERROR writing to socket");
    
    bzero(buffer,256);
    n = read(sockfd,buffer,255); //read from the socket
    if (n < 0) 
         error("ERROR reading from socket");
    printf("%s\n",buffer);
    
    close(sockfd); //close socket
    
    return 0;
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
            write(sockfd,file_data,PACKET_SIZE);
            }
          else{
            write(sockfd,file_data,PACKET_SIZE);
            if (feof(filepointer)){
                printf("End of file.\n");
            }
            if (ferror(filepointer)){
                printf("Error reading this file.\n");
            }
          }
      }

      fclose(filepointer); //close the IO stream for the file 
      free(file_data);
}