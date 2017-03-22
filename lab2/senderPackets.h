void error(char* msg);
void sendFile(char* filename, int sockfd, struct sockaddr_in client_addr, socklen_t clilen, int probOfLoss);
void makePacket(char* file_data, int headerType, int sequenceNumber, FILE* filepointer, int maxSeqNum);
void createDataHeader(char* filebuffer, int headerType, int sequenceNumber, int maxSequenceNumber, int filesize);
int determineIfPacketWasDropped(int probOfLoss);
int waitForAck(int sockfd, struct sockaddr_in client_addr, socklen_t clilen, char* packetBuffer, int probOfLoss);
void readHeaderAndACK(char* packetBuffer, int* packetType, int* sequenceNumber, int* maxSequenceNumber, int* datasize);
