/* 
   Dylan Brown
   Lab 1
   CSE 2431

   A simple HTTP server.
*/

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
void sendHTMLFile(char* filename, int newsockfd);
void sendImageFile(char* filename, int contentType, int newsockfd);
void sendErrorResponse(int newsockfd);

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
	
	int option = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //this code allows the socket to bind even though it's in TIME_WAIT

	bzero((char *) &serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY; //for the server the IP address is always the address that the server is running on
    serv_addr.sin_port = htons(portno); //convert from host to network byte order

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) //Bind the socket to the server address
              error("ERROR on binding");

    	listen(sockfd,5); // Listen for socket connections. Backlog queue (connections to wait) is 5
    	clilen = sizeof(cli_addr);

    while(1){

          1) Block until a new connection is established
        /*accept function: 
        */
          2) the new socket descriptor will be used for subsequent communication with the newly connected client.
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (newsockfd < 0) 
            error("ERROR on accept");
        
        int n;
        bzero(request,512);

        n = read(newsockfd,request,512); //Read is a block function. It will read at most 255 bytes
		if (n < 0) error("ERROR reading from socket");
		sendHTTPResponse(request, newsockfd); 
		    close(newsockfd);
   
    }  
    close(sockfd);
    close(newsockfd);
    printf("Closing connection. Goodbye!\n");
    return 0;
}

void sendHTTPResponse(char* request, int newsockfd){

	//Check for a specific substring in the request
	char* text_html_request = "GET /text.html HTTP/1.1\r\n";
	char* text_html_request_true = strstr(request, text_html_request);

	char* picture_html_request = "GET /picture.html HTTP/1.1\r\n";
	char* picture_html_request_true = strstr(request, picture_html_request);

	char* sample_image_request = "GET /sample2.jpg HTTP/1.1\r\n";
	char* sample_image_request_true = strstr(request, sample_image_request);

	char* big_picture_html_request = "GET /bigpicture.html HTTP/1.1\r\n";
	char* big_picture_html_request_true = strstr(request, big_picture_html_request);

	char* big_picture_image_request = "GET /bigpicture.jpeg HTTP/1.1\r\n";
	char* big_picture_image_request_true = strstr(request, big_picture_image_request);

	char* sample_image_request_gif = "GET /sample2.gif HTTP/1.1\r\n";
	char* sample_image_request_gif_true = strstr(request, sample_image_request_gif);	

	char* big_picture_image_request_gif = "GET /bigpicture.gif HTTP/1.1\r\n";
	char* big_picture_image_request_gif_true = strstr(request, big_picture_image_request_gif);


	//if the substring exists, handle the particular request
	if (text_html_request_true){
	  sendHTMLFile("text.html", newsockfd);
	}
	else if (picture_html_request_true){
	  sendHTMLFile("picture.html", newsockfd);
	}
	else if (sample_image_request_true){
	  sendImageFile("sample2.jpg", 1, newsockfd);
	}
	else if (big_picture_html_request_true){
	  sendHTMLFile("bigpicture.html", newsockfd);
	}
	else if (big_picture_image_request_true){
	  sendImageFile("bigpicture.jpeg", 1, newsockfd);
	}
	else if (sample_image_request_gif_true){
	  sendImageFile("sample2.gif", 2, newsockfd);
	}
	else if (big_picture_image_request_gif_true){
	  sendImageFile("bigpicture.gif", 2, newsockfd);
	}
	else{
	  sendErrorResponse(newsockfd);
	}
} 



void sendHTMLFile(char* filename, int newsockfd){

	  char* str = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n";
	  int len = strlen(str);
	  write(newsockfd,str,len);

	  long fsize;
	  FILE *filepointer = fopen(filename, "rb"); //Open file stream for the text.html file

	  if (!filepointer){
	  	perror("The html file cannot be opened.");
	  	exit(1);
	  }

	  fseek(filepointer, 0, SEEK_END); //Move "cursor" to the end of the file
	  fsize = ftell(filepointer); //Get the position of the cursor as the length of the file

	  if (fsize == -1){
	  	perror("The html file size cannot be retrieved.");
	  	exit(1);
	  }
	  
	  str = "Content-Length: ";
	  len = strlen(str);
	  write(newsockfd,str,len);

	  char sizebuffer[64];
	  int n = sprintf(sizebuffer, "%lu", fsize);
	  if (n < 0){
	  	perror("Error converting fsize to char array.");
	  	exit(1);
	  }
	  len = strlen(sizebuffer);
	  write(newsockfd,sizebuffer,len);
	  

	  str = "\r\nContent-Type: text/html; charset=utf-8\r\n\r\n";
	  len = strlen(str);
	  write(newsockfd,str,len);

	  rewind(filepointer); //Move cursor back to start

	  char *html_data = (char*) malloc(fsize); //Allocate the memory for the size of the html_data

	  if (!html_data){
	  	perror("The file buffer could not be allocated in memory.");
	  	exit(1);
	  }


	  if (fread(html_data, fsize, 1, filepointer) == 0){
	  	perror("The file was not successfully read.");
	  	exit(1);
	  }

	  len = strlen(html_data);
	  write(newsockfd,html_data,len);

	  fclose(filepointer); //close the IO stream for the file 

	  free(html_data);
}

void sendImageFile(char* filename, int contentType, int newsockfd){

	  char* str = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\n";
	  int len = strlen(str);
	  write(newsockfd,str,len);

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

	  str = "Transfer-Encoding: chunk\r\n";
	  len = strlen(str);
	  write(newsockfd,str,len);

	  if (contentType == 1){
	  str = "Content-Type: image/jpeg\r\n\r\n";
	  len = strlen(str);
	  write(newsockfd,str,len);
	  }
	  else if (contentType == 2){
	  str = "Content-Type: image/gif\r\n\r\n";
	  len = strlen(str);
	  write(newsockfd,str,len);
	  }

	  rewind(filepointer); //Move cursor back to start

	  char *image_data = (char*) malloc(fsize); //Allocate the memory for the size of the image_data

	  if (!image_data){
	  	perror("The file buffer could not be allocated in memory.");
	  	exit(1);
	  }

	  int j = 1;
	  while(j){
		  int n = fread(image_data, 1024, 1, filepointer);
		  if (n > 0){
		  	write(newsockfd,image_data,1024);
		  	}
		  else{
		  	write(newsockfd,image_data,1024);
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
	  free(image_data);
}


void sendErrorResponse(int newsockfd){
	char* str = "HTTP/1.1 404 Not Found\r\nConnection: close\r\nContent-type: text/html\r\n\r\n<html><body><h1>404 NOT FOUND</h2></body></html>";
	int len = strlen(str);
	write(newsockfd,str,len);
}
