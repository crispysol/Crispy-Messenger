/*
 * client.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: mihail, andreea, radu
 */

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerFunctions.h"
#include "Client.h"

// Defines
#define BANNER			"OK"

using namespace std;

int client_port = 0;
Client * client;

static void process_server_msg(char * buffer, int & fdmax, fd_set * read_fds);

/**
 * Process message from server.
 */
static void process_server_msg(string buffer, int & fdmax, fd_set * read_fds) {
	if (buffer.find(SUCCESS_MSG) == 0)
		return;

	if(buffer.find(CMD_CONN_CLIENT_TO_CLIENT_RES) == 0) {
		// connect with user
		printf("buffer %s\n", buffer.c_str());
		int sockfd = client->connect_with_user_res(buffer, fdmax, read_fds);
		dprintf("[DONE, sockfd %i]%s\n", sockfd, buffer.c_str());
		if (sockfd == -1)
			return;
			
		//tell the other end (client to connect to) my username
		char msg[BUFFER_LENGTH];
		sprintf(msg, "%s %s", CMD_CONN_REQ_FROM, client->get_username().c_str());
		assert(send(sockfd, msg, strlen(msg) + 1, 0) >= 0);
		
		return;
	}
}
/**
 * Execute command received from STDIN
 */
void stdin_command(Client *client, fd_set * read_fds) {
	string line;
	getline(cin, line);

	// Tratarea comenzii de la tastatura
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
		string username = line.substr(user_pos, pass_pos -1 - user_pos);
		client->authentication(
				username,
				line.substr(pass_pos));
		client->set_username(username);
		return;
	}
	if(line.find(CMD_ADD_USER) == 0){
		int user_pos = line.find(" ") + 1;
		client->add_user(
				line.substr(user_pos));
		return;
	}

	if(line.find(CMD_REMOVE_USER) == 0) {
		int user_pos = line.find(" ") + 1;
		client->remove_user(line.substr(user_pos));
		return;
	}
	
	if(line.find(CMD_ADD_GROUP) == 0) {
		int group_pos = line.find(" ") + 1;
		client->add_group(line.substr(group_pos));
		return;
	}
	
	if(line.find(CMD_CONN_CLIENT_TO_CLIENT_REQ) == 0) {
		int user_pos = line.find(" ") + 1;
		client->connect_with_user_req(line.substr(user_pos));
		return;
	}

	if (line.find(CMD_GET_PROFILE) == 0) {
		
		client->get_profile(line.substr(line.find(" ") + 1));
		return;
	}

	if (line.find(CMD_UPDATE_PROFILE) == 0) {
		//TODO
		return;
	}

	if (line.find(EXIT_MSG) == 0) {
		//TODO end all connections
		//end connection to server
		end_connection(client->get_server_socket(), read_fds);
		exit(EXIT_SUCCESS);
	}
}

static void client_command(char * buffer, int sockfd) {	

	if (strstr(buffer, SUCCESS_MSG) == 0) {
		dprintf("just start tweeting...;)\n");
		return;
	}
	
	if (strstr(buffer, ERR_MSG) == 0) {
		dprintf("cannot start tweeting :( error encountered\n");
		return;
	}
		
	if (strstr(buffer, CMD_CONN_REQ_FROM) == 0) {
		//get username
		char *username = strchr(buffer, ' ');
		if (!username) {
			dprintf("%s wrong format (correct format is '%s username')\n", CMD_CONN_REQ_FROM, CMD_CONN_REQ_FROM);
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
			return;
		}
		
		client->insert_in_connected_users(username, sockfd);
		assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);
		
		return;
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
	client = new Client(socket_server);

	// Main loop
	for (;;) {
		tmp_fds = read_fds;
		assert(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) != -1);

		// Check every socket for new data
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				// New connection
				if (i == sockfd) {
					dprintf("received new connection on %i\n", i);
					new_connection(sockfd, fdmax, &read_fds, ip, newsockfd, newport);
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
					process_server_msg(string(buffer), fdmax, &read_fds);
					continue;
				}

				// Received data from another client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					assert(n == -1);
					end_connection(i, &read_fds);
					client->remove_from_connected_users(i);
				} else {
					dprintf("received from client: %s\n", buffer);
					client_command(buffer, i);
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
