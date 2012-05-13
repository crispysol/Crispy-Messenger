/*
 * Functions.cpp
 *
 *  Created on: May 5, 2011
 *      Author: cmihail, andreea, liviu, radu
 */

#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <netdb.h>

#include "Server.h"

using namespace std;

// Defines
#define MAX_CLIENTS		100
#define BANNER			"OK"

ClientInfo::ClientInfo(string ip, int port) : ip(ip), port(port)
{
	this->set_username("");
}

ClientInfo::~ClientInfo() {}

string ClientInfo::get_ip() {
	return ip;
}

int ClientInfo::get_port() {
	return port;
}

/**
 * Receive a new connection and add it to read_fds. Also return ip, new socket descriptor and port of this socket, so
 * that the server can keep track of connections (i.e. map with key newsockfd, so that when something is received on 
 * newsockfd, the server can send back the corresponding ip and port).
 */
void new_connection(int sockfd, int & fdmax, fd_set * read_fds, string &ip, int &newsockfd, int &newport) {
	struct sockaddr_in cli_addr;
	int clilen = sizeof(cli_addr);

	// Accept new connection
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
			(socklen_t *) &clilen);
	assert(newsockfd != -1);

	// Add new socket to read_fds
	FD_SET(newsockfd, read_fds);
	if (newsockfd > fdmax) {
		fdmax = newsockfd;
	}

	ip = string(inet_ntoa(cli_addr.sin_addr));
	newport = cli_addr.sin_port;
	cout << "[SERVER] New connection from: " << ip << " on port " << newport << endl;

	// Send 'welcome' message
	assert(send(newsockfd, BANNER, strlen(BANNER) + 1, 0) >= 0);
}

/**
 * End a connection and remove it from read_fds
 */
void end_connection(int sockfd, fd_set * read_fds) {

	// End of connection
	cout << "MESAJ NOU\n[SERVER] Socket " << sockfd << " inchis\n";

	// Sterg clientul din baza de date.
	//delete_client(i, &database); TODO

	// Delete client
	FD_CLR(sockfd, read_fds);
	close(sockfd);
}

/**
 * Init server
 */
void init_server(int & server_port, int & sockfd, int & fdmax, fd_set * read_fds) {
	struct sockaddr_in serv_addr;

	// File descriptors used for select()
	FD_ZERO(read_fds);

	// Adaug STDIN in multimea read_fds
	FD_SET(STDIN_FILENO, read_fds);
	fdmax = STDIN_FILENO;

	// Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	assert(sockfd >= 0);

	// Create server
	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;

	// Use localhost public IP
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(server_port);

	assert(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(struct sockaddr)) >= 0);

	// Find port on which server listens (only for client)
	if (server_port == 0) {
		socklen_t len = sizeof(serv_addr);
		assert(getsockname(sockfd, (struct sockaddr *) &serv_addr, &len) != -1);
		server_port = ntohs(serv_addr.sin_port);
	}

	// Socket used for listening
	listen(sockfd, MAX_CLIENTS);
	FD_SET(sockfd, read_fds);
	if (fdmax < sockfd) {
		fdmax = sockfd;
	}
}

/**
 * Connect to a server (only for client)
 */
void connect_to_server(char * server_ip, int server_port,  int & socket_server, int & fdmax, fd_set * read_fds) {
	struct sockaddr_in  serv_addr;
	struct hostent *server;

	dprintf("establish new connection for ip %s and port %i\n", server_ip, server_port);

	// Create socket
	socket_server = socket(AF_INET, SOCK_STREAM, 0);
	assert(socket_server >= 0);

	// Obtain details about server
	server = gethostbyname(server_ip);
	if (server == NULL) {
		cerr << "ERROR, no such host" << endl;
		exit(0);
	}

	memset((char *) &serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	memcpy((char *) &serv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	serv_addr.sin_port = htons(server_port);

	// Connect to server
	assert(connect(socket_server,(struct sockaddr*) &serv_addr, sizeof(serv_addr)) >= 0);

	// Add fd to read_fds
	FD_SET(socket_server, read_fds);
	if (fdmax < socket_server)
		fdmax = socket_server;
}

/**
 * Tokenizer
 */
void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ") {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}
