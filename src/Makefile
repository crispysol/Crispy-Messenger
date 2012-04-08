CC = g++
CFLAGS = -Wall -g
CPPFLAGS = -D_DEBUG
 
all: ServerMain ClientMain ClientGTK

ServerMain: ServerMain.o ServerFunctions.o Server.o
	$(CC) $(CFLAGS) $^ -l mysqlcppconn -o $@

ClientMain: ClientMain.o ServerFunctions.o Client.o

ClientGTK: ClientGTK.o ServerFunctions.o
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -o $@ `pkg-config --libs gtk+-3.0`

ClientGTK.o: ClientGTK.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`

CreateDatabase: CreateDatabase.o
	$(CC) $(CFLAGS) $^ -l mysqlcppconn -o $@

ServerMain.o: ServerMain.cpp

Server.o: Server.cpp

clean: 
	rm -rf *.o ServerMain ClientMain CreateDatabase
