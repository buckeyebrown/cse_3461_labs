# Makefile for minilab1
# CSE 3461
# Dylan Brown

all: server client

server: server.o
	gcc -o server server.o

client: client.o
	gcc -o client client.o

server.o: server.c
	gcc -c -Wall server.c

client.o: client.c
	gcc -c -Wall client.c