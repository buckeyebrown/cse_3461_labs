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

void sendHTTPResponse(char* request, int newsockfd);
void readHTMLFile(char* filename, int newsockfd);

int main(int argc, char *argv[]){
    int sockfd, newsockfd; //descriptors rturn from socket and accept system calls
    int portno = 5434; // port number 5434
    socklen_t clilen;
     
    char request[512];
     
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
        bzero(request,512);

        n = read(newsockfd,request,512); //Read is a block function. It will read at most 255 bytes
		if (n < 0) error("ERROR reading from socket");

		printf("%s\n", request);

		sendHTTPResponse(request, newsockfd);

        //n = write(newsockfd,response,1024); //NOTE: write function returns the number of bytes actually sent out Ñ> this might be less than the number you told it to send

        printf("Closing connection. Goodbye!\n");
        close(sockfd);
        close(newsockfd);
        return 0; 
    }     
}

void sendHTTPResponse(char* request, int newsockfd){

	char* text_html_request = "GET /text33.html HTTP/1.1\r\n";
	char* text_html_request_true = strstr(request, text_html_request);

	char* picture_html_request = "GET /picture.html HTTP/1.1\r\n";
	char* picture_html_request_true = strstr(request, picture_html_request);

	char* big_picture_html_request = "GET /bigpicture.html HTTP/1.1\r\n";
	char* big_picture_html_request_true = strstr(request, big_picture_html_request);


	if (text_html_request_true){
	  readHTMLFile("text.html", newsockfd);
	}
	else if (picture_html_request_true){

	}
	else if (big_picture_html_request_true){

	}
	else{

	}
} 

void readHTMLFile(char* filename, int newsockfd){

	  char* str = "HTTP/1.1 200 OK\r\n";
	  int len = strlen(str);
	  write(newsockfd,str,len);

	  long fsize;
	  FILE *filepointer = fopen(filename, "rb"); //Open file stream for the text.html file

	  if (!filepointer){
	  	perror("The text.html file cannot be opened.");
	  	exit(1);
	  }

	  fseek(filepointer, 0, SEEK_END); //Move "cursor" to the end of the file
	  fsize = ftell(filepointer); //Get the position of the cursor as the length of the file

	  if (fsize == -1){
	  	perror("The text.html file size cannot be retrieved.");
	  	exit(1);
	  }

	  /**
	  str = "Content-length: ";
	  len = strlen(str);
	  write(newsockfd,str,len);
	  printf(" works here\n");

	  char* size = (char*) fsize;
	  len = strlen(size);
	  write(newsockfd,size,len);
	  printf(" works here\n");
	  */

	  str = "\r\nContent-type: text/html\r\n";
	  len = strlen(str);
	  write(newsockfd,str,len);
	  printf(" works here\n");

	  //responseMessage = responseMessage + "Content-length: " + &fsize + "\r\n"; //print the fsize as the content length
	  //responseMessage = responseMessage + "Content-type: text/html\r\n";
	  rewind(filepointer); //Move cursor back to start

	  char *html_data = (char*) malloc(fsize); //Allocate the memory for the size of the html_data

	  if (!html_data){
	  	perror("The file buffer could not be allocated in memory.");
	  	exit(1);
	  }

	  //fread(html_data, fsize, 1, filepointer) == 0;

	  if (fread(html_data, fsize, 1, filepointer) == 0){
	  	perror("The file was not successfully read.");
	  	exit(1);
	  }

	  printf("\n\n\n");
	  len = strlen(html_data);
	  printf("%i\n", len);
	  write(newsockfd,html_data,len);
	  printf(" works here\n");

	  fclose(filepointer); //close the IO stream for the file 
	  printf(" works here\n");

	  free(html_data);
	  //responseMessage = responseMessage + html_data;
}