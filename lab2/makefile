# Makefile for Lab 2
# CSE 3461
# Dylan Brown

all: sender receiver

receiver: receiver.o packets.o
	gcc -o receiver receiver.o

receiver.o: receiver.c packets.h
	gcc -c -Wall receiver.c

sender: sender.o packets.o
	gcc -o sender sender.o

sender.o: sender.c packets.h
	gcc -c -Wall sender.c

packets.o: packets.c packets.h
	gcc -c -Wall packets.c