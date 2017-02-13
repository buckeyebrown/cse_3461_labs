#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

void error(char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]){
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno = 5434; // port number 5434
    socklen_t clilen;
     
    char buffer[256];
     
    /*sockaddr_in: Structure Containing an Internet Address*/
    struct sockaddr_in serv_addr, cli_addr;

    /*Create a new socket
    AF_INET: Address Domain is Internet 
    SOCK_STREAM: Socket Type is STREAM Socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       error("ERROR opening socket");
	
	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");

    while(1){
        listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
    	clilen = sizeof(cli_addr);
        /*accept function: 
          1) Block until a new connection is established
          2) the new socket descriptor will be used for subsequent communication with the newly connected client.
        */
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (newsockfd < 0) 
            error("ERROR on accept");
        
        int n;
        bzero(buffer,256);

        n = read(newsockfd,buffer,255); //Read is a block function. It will read at most 255 bytes
		if (n < 0) error("ERROR reading from socket");

		printf("%s\n", buffer);


        n = write(newsockfd,buffer,255); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send

        printf("Closing connection. Goodbye!\n");
        close(sockfd);
        close(newsockfd);
        return 0; 
    }     
     
}