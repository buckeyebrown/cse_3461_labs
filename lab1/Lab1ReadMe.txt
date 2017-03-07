Dylan Brown
CSE 3461
Lab 1 Readme
200148626

Compile the program with the command "make"
Run the program with ./webserver in the same directory the program is located in

The port used is port 5434. You can access the webserver with localhost:5434. 

In task 1, the HTTP request will be responded with a small HTML file. In task 2, the response will be a small html file with a small image. In task 3, it will be a small HTML file with a large image.

Issues that I came across when writing this program was sending the image file over a large enough buffer, an issue I eventually solved by sending it via batches/chunks of 1024 byte buffers. Another issue was allowing the browser to send multiple requests. I solved this by closing the newsockfd inside my while loop.
