/*
 * server.cpp
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
int run_server(int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, fdmax, n;
	fd_set read_fds, tmp_fds;
	FD_ZERO(&tmp_fds);

	// Init server
	init_server(server_port, sockfd, fdmax, &read_fds);

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

				// Received data from client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					assert(n == 0);
					end_connection(i, &read_fds);
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
	if (argc != 2) {
		cerr << "Usage: ./server.cpp port" << endl;
		exit(EXIT_FAILURE);
	}

	// Get port number
	server_port = atoi(argv[1]);

	// Initialize server
	run_server(server_port);
}
