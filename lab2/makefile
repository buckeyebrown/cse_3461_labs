# Makefile for Lab 2
# CSE 3461
# Dylan Brown

all: sender receiver

receiver: receiver.o
	gcc -o receiver receiver.o

receiver.o: receiver.c
	gcc -c -Wall receiver.c

sender: sender.o
	gcc -o sender sender.o

sender.o: sender.c
	gcc -c -Wall sender.c

