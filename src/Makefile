CC = g++
CFLAGS = -Wall -g
#g++ test.c -o test -l mysqlcppconn
all: ServerMain ClientMain

ServerMain: ServerMain.o ServerFunctions.o

ClientMain: ClientMain.o ServerFunctions.o

clean: 
	rm -rf *.o ServerMain ClientMain
