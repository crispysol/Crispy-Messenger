/*
 * Functions.cpp
 *
 *  Created on: May 5, 2011
 *      Author: cmihail
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

using namespace std;

// Defines
#define BUFFER_LENGTH	256
#define MAX_CLIENTS		100
#define BANNER			"OK"

/**
 * Receive a new connection and add it to read_fds
 */
void new_connection(int sockfd, int & fdmax, fd_set * read_fds) {
	struct sockaddr_in cli_addr;
	int clilen = sizeof(cli_addr), newsockfd;

	// Accept new connection
	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
			(socklen_t *) &clilen);
	assert(newsockfd != -1);

	// Add new socket to read_fds
	FD_SET(newsockfd, read_fds);
	if (newsockfd > fdmax) {
		fdmax = newsockfd;
	}

	cout << "[SERVER] New connection from: " << inet_ntoa(cli_addr.sin_addr) << endl;

	// Send 'welcome' message
	assert(send(newsockfd, BANNER, strlen(BANNER), 0) >= 0);
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
	close(sockfd);
	FD_CLR(sockfd, read_fds);
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
