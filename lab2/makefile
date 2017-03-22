# Makefile for Lab 2
# CSE 3461
# Dylan Brown

all: sender receiver

receiver: receiver.o receiverPackets.o
	gcc -o receiver receiver.o receiverPackets.o

receiver.o: receiver.c receiverPackets.h
	gcc -c -Wall receiver.c 

sender: sender.o senderPackets.o
	gcc -o sender sender.o senderPackets.o

sender.o: sender.c senderPackets.h
	gcc -c -Wall sender.c

senderPackets.o: senderPackets.c senderPackets.h
	gcc -c -Wall senderPackets.c

receiverPackets.o: receiverPackets.c receiverPackets.h
	gcc -c -Wall receiverPackets.c