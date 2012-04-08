/*
 * server.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: mihail, andreea
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerFunctions.h"
#include "Server.h"

// Defines
#define BANNER			"OK"

using namespace std;

/**
 * Execute command received from STDIN
 */
void stdin_command() {
	string line;
	getline(cin, line);

	// Tratarea comenzii de la tastatura TODO
		if (line.find(EXIT_MSG) == 0) {
			//TODO end all connections
			exit(EXIT_SUCCESS);
		}
}

void client_command(string line, int sockfd, Server server) {

	if (line.find(CMD_REGISTER) == 0) {
			int user_pos = line.find(" ") + 1,
				pass_pos = line.find(" ", user_pos) + 1,
				email_pos = line.find(" ", pass_pos) + 1;
			server.register_client(sockfd,
					line.substr(user_pos, pass_pos -1 - user_pos),
					line.substr(pass_pos, email_pos - 1 - pass_pos),
					line.substr(email_pos));
			return;
		}
		if (line.find(CMD_AUTH) == 0) {
			int user_pos = line.find(" ") + 1,
				pass_pos = line.find(" ", user_pos) + 1,
				ip_pos = line.find(" ", pass_pos) + 1,
				port_pos = line.find(" ", ip_pos);
			server.authentication(sockfd,
					line.substr(user_pos, pass_pos -1 - user_pos),
					line.substr(pass_pos, ip_pos - 1 - pass_pos),
					line.substr(ip_pos, port_pos - 1 - ip_pos),
					atoi(line.substr(port_pos).c_str()));
			return;
		}

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

	Server server = Server();

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
					//send(i, "TEST", strlen("TEST"), 0); // TODO delete
					continue;
				}

				// Execute command from STDIN
				if (i == STDIN_FILENO) {
					stdin_command();
					continue;
				}

				// Received data from client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					//assert(n == 0); TODO
					end_connection(i, &read_fds);
				} else {
					//client_command(buffer, i, inet_ntoa(cli_addr.sin_addr),
						//	&database, result);TODO
					client_command(string(buffer), i, server);
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
	if (argc < 2) {
		cerr << "Usage: ./server.cpp port" << endl;
		exit(EXIT_FAILURE);
	}

	// Get port number
	server_port = atoi(argv[1]);

	// Initialize server
	run_server(server_port);
}
