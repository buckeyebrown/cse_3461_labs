void error(char* msg);
int checkIfSequenceIsDuplicate(int sequenceNumber, int* previousSequenceNumbers, int maxSequenceNumber);
int readHeaderAndData(char* packetBuffer, char* dataBuffer, int* packetType, int* maxSequenceNumber, int* datasize);
int determineIfPacketWasDropped(int probOfLoss);
char* concat(const char *string_1, const char *string_2);
int readHeaderAndData(char* packetBuffer, char* dataBuffer, int* packetType, int* maxSequenceNumber, int* datasize);
void readHeaderAndACK(char* packetBuffer, int* packetType, int* sequenceNumber, int* maxSequenceNumber, int* datasize);
void sendAck(int sequenceNumber, int *maxSequenceNumber, struct sockaddr_in serv_addr, socklen_t addrlen, int sockfd);