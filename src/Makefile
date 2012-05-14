CC = g++
CFLAGS = -Wall -g
CPPFLAGS = -D_DEBUG

.PHONY: all clean
 
all: ServerMain ClientMain ClientGTK

# Main programs

ServerMain: ServerMain.o ServerFunctions.o Server.o User.o Profile.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -l mysqlcppconn -o $@

ClientMain: ClientMain.o ServerFunctions.o Client.o User.o Profile.o
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -l json_linux-gcc-4.6.1_libmt -o $@

ServerMain.o: ServerMain.cpp
Server.o: Server.cpp

# Interface
ClientGTK: User.o Profile.o Client.o ClientGTK.o LoginWindow.o MainWindow.o ChatWindow.o ServerFunctions.o GTKFunctions.o Signal.o
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -l json_linux-gcc-4.6.1_libmt -o $@ `pkg-config --libs gtk+-3.0`
ClientGTK.o: Interface/ClientGTK.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) $(CPPFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
LoginWindow.o: Interface/LoginWindow.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
MainWindow.o: Interface/MainWindow.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
ChatWindow.o: Interface/ChatWindow.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
GTKFunctions.o: Interface/GTKFunctions.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
Signal.o: Interface/Signal.cpp
	$(CC) `pkg-config --cflags gtk+-3.0` $^ $(CFLAGS) -c -o $@ `pkg-config --libs gtk+-3.0`
	
# Database creation		
CreateDatabase: CreateDatabase.o
	$(CC) $(CFLAGS) $^ -l mysqlcppconn -o $@

clean: 
	rm -rf *.o ServerMain ClientMain CreateDatabase ClientGTK
