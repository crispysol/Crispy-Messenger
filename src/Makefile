CC = g++
CFLAGS = -Wall -g

all: Server Client

Server: Server.o ServerFunctions.o

Client: Client.o ServerFunctions.o

clean: 
	rm -rf *.o Server Client
