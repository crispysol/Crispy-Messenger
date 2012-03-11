CC = g++
CFLAGS = -Wall -g
 
all: ServerMain ClientMain

ServerMain: ServerMain.o ServerFunctions.o
	$(CC) $(CFLAGS) $^ -l mysqlcppconn -o $@

ClientMain: ClientMain.o ServerFunctions.o

CreateDatabase: CreateDatabase.o
	$(CC) $(CFLAGS) $^ -l mysqlcppconn -o $@

clean: 
	rm -rf *.o ServerMain ClientMain CreateDatabase
