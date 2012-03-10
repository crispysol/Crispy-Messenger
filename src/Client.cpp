/*
 * client.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: mihail
 */

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerFunctions.h"

// Defines
#define BUFFER_LENGTH	256
#define BANNER			"OK"

using namespace std;

/**
 * Execute command received from STDIN
 */
void stdin_command() {
	char buffer[BUFFER_LENGTH];
	assert(read(STDIN_FILENO, buffer, BUFFER_LENGTH) > 0);

	char *s = strchr(buffer, '\n');
	buffer[s - buffer] = '\0';

	// Tratarea comenzii de la tastatura TODO
	cout << buffer << endl; // TODO delete
}

/**
 * Initialize server
 */
int run_server(char * server_ip, int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, socket_server, fdmax, n;
	fd_set read_fds, tmp_fds;
	FD_ZERO(&tmp_fds);

	// Init localhost server (communication with other clients
	init_server(socket_server, sockfd, fdmax, &read_fds);

	// Connect to main server
	connect_to_server(server_ip, server_port, socket_server, fdmax, &read_fds);

	// Main loop
	for (;;) {
		tmp_fds = read_fds;
		assert(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) != -1);

		// Check every socket for new data
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				// New connection
				if (i == sockfd) {
					new_connection(sockfd, fdmax, &read_fds);
					continue;
				}

				// Execute command from STDIN
				if (i == STDIN_FILENO) {
					stdin_command();
					continue;
				}

				// Receive data from server
				if (i == server_port) {
					// TODO;
					cout << "RECEIVED" << endl;
					continue;
				}

				// Received data from another client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					assert(n == 0);
					end_connection(i, &read_fds);
					// TODO if (i == server_port) => quit application and free all resource (sockets + etc)
				} else {
					//client_command(buffer, i, inet_ntoa(cli_addr.sin_addr),
						//	&database, result);
					cout << "TODO" << endl; //
				}
			}
		}
	}

	close(sockfd);
	return 0;
}

/**
 * Functia main.
 */
int main(int argc, char **argv) {
	int server_port;

	// Check if number of arguments is correct
	if (argc != 3) {
		cerr << "Usage: ./server.cpp server_ip port" << endl;
		exit(EXIT_FAILURE);
	}

	// Get port number
	server_port = atoi(argv[2]);

	// Initialize server
	run_server(argv[1], server_port);
}
