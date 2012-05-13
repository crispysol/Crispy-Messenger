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
#include "Client.h"

// Defines
#define BANNER			"OK"

using namespace std;

int client_port = 0;

/**
 * Execute command received from STDIN
 */
void stdin_command(Client *client, fd_set * read_fds) {
	string line;
	getline(cin, line);

	// Tratarea comenzii de la tastatura TODO
	if (line.find(CMD_REGISTER) == 0) {
		int user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1,
			email_pos = line.find(" ", pass_pos) + 1;
		client->register_client(
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos, email_pos - 1 - pass_pos),
				line.substr(email_pos));
		return;
	}
	if (line.find(CMD_AUTH) == 0) {
		int user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1;
		client->authentication(
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos));
		return;
	}
	if(line.find(CMD_ADD_USER) == 0){
		int user_pos = line.find(" ") + 1;
		client->add_user(
				line.substr(user_pos));
		return;
	}
	cout << CMD_REMOVE_USER << " send to server " << endl << flush;
	if(line.find(CMD_REMOVE_USER) == 0) {
		int user_pos = line.find(" ") + 1;
		client->remove_user(line.substr(user_pos));
		return;
	}
	if (line.find(EXIT_MSG) == 0) {
		//TODO end all connections
		//end connection to server
		end_connection(client->get_server_socket(), read_fds);
		exit(EXIT_SUCCESS);
	}
}

/**
 * Initialize server
 */
int run_server(char * server_ip, int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, socket_server, fdmax, n, newsockfd, newport;
	string ip;
	fd_set read_fds, tmp_fds;
	FD_ZERO(&tmp_fds);

	// Init localhost server (communication with other clients
	init_server(client_port, sockfd, fdmax, &read_fds);

	// Connect to main server
	connect_to_server(server_ip, server_port, socket_server, fdmax, &read_fds);

	// Client instance
	Client *client = new Client(socket_server);

	// Main loop
	for (;;) {
		tmp_fds = read_fds;
		assert(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) != -1);

		// Check every socket for new data
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				// New connection
				if (i == sockfd) {
					new_connection(sockfd, fdmax, &read_fds, ip, newsockfd, newport);
					client->insert_in_sockfd_to_clients(newsockfd, new ClientInfo(ip, newport));
					continue;
				}

				// Execute command from STDIN
				if (i == STDIN_FILENO) {
					stdin_command(client, &read_fds);
					continue;
				}

				// Receive data from server
				if (i == socket_server) {
					n = recv(i, buffer, sizeof(buffer), 0);
					assert(n >= 0); // TODO test if we have to exit
					dprintf("[CLIENT]received from SERVER: %s\n", buffer);
					continue;
				}

				// Received data from another client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					assert(n == -1);
					end_connection(i, &read_fds);
					//TODO remove connection from connected_users list
				} else {
					//client_command(buffer, i, inet_ntoa(cli_addr.sin_addr),
						//	&database, result); TODO
					cout << "TODO" << buffer << endl; //
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
