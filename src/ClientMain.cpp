/*
 * client.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: mihail, andreea, radu, liviu
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
static void client_command(string buffer, int sockfd);
static void stdin_command(Client *client, fd_set * read_fds);

/**
 * Process message from server.
 */
static void process_server_msg(string buffer, int & fdmax, fd_set * read_fds) {
	if (buffer.find(SUCCESS_MSG) == 0)
		return;

	if(buffer.find(CMD_CONN_CLIENT_TO_CLIENT_RES) == 0) {
		// connect with user
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
	
	if (buffer.find(CMD_SEND_MSG) == 0) {
		int 	src_pos = buffer.find(" ") + 1,
			msg_pos = buffer.find(" ", src_pos) + 1;
		string msg, src;
		
		src = buffer.substr(src_pos, msg_pos - 1 - src_pos);
		msg = buffer.substr(msg_pos);
		dprintf("received message %s from %s\n", msg.c_str(), src.c_str());
		//TODO send to chat window
		
	}
}

/**
 * Execute command received from STDIN
 */
static void stdin_command(Client *client, fd_set * read_fds) {
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


	if(line.find(CMD_DEL_GROUP) == 0) {
		int group_pos = line.find(" ") + 1;
		client->remove_group(line.substr(group_pos));
		return;
	}
	if(line.find(CMD_MV_USER) == 0) {
		int user_pos= line.find(" ") + 1,
			group_pos = line.find(" ",user_pos) + 1;
			
		client-> move_user_to_group(
						line.substr(user_pos, group_pos-1 - user_pos),
						line.substr(group_pos)
						);
		return;
	}	

	if (line.find(CMD_GET_PROFILE) == 0) {
		
		client->get_profile(line.substr(line.find(" ") + 1));
		return;
	}

	if (line.find(CMD_UPDATE_PROFILE) == 0) {
		int	name_pos = line.find(" ") + 1,
			sname_pos = line.find(" ", name_pos) + 1,
			phone_pos = line.find(" ", sname_pos) + 1,
			hobb_pos = line.find(" ", phone_pos) + 1;

		client->update_profile(
			line.substr(name_pos, sname_pos - name_pos - 1),
			line.substr(sname_pos, phone_pos - sname_pos - 1),
			line.substr(phone_pos, hobb_pos - phone_pos - 1),
			line.substr(hobb_pos));
	
		return;
	}

	if (line.find(CMD_SEND_MSG) == 0) {
		int 	name_dst = line.find(" ") + 1,
			msg_pos = line.find(" ", name_dst) + 1;
		//Client communicate through server
		client->send_message( 
			line.substr(name_dst, msg_pos - 1 - name_dst), 
			line.substr(msg_pos));
	}
	
	if(line.find(CMD_SET_STATE) == 0) {
		int state_pos= line.find(" ") + 1;
			
			
		client-> send_state(line.substr(state_pos)
						);
		return;
	}

	if(line.find(CMD_SET_STATUS) == 0) {
		int state_pos= line.find(" ") + 1;
			
			
		client-> send_status(line.substr(state_pos)
						);
		return;
	}
	
	
	if(line.find(CMD_SEARCH_USER ) == 0) {
		int criteria_pos= line.find(" ") + 1;
			
			
		string result=client-> search_user(line.substr(criteria_pos)
						);
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
 * Process message from other client.
 */
static void client_command(string buffer, int sockfd) {	
	if (buffer.find(CMD_CONN_REQ_FROM) == 0) {
		//get username of client which initiated connection
		char username[BUFFER_LENGTH];
		int user_pos = buffer.find(' ') + 1;
		
		if (user_pos == 0) {
			dprintf("[CLIENT]%s wrong format (correct format is '%s username')\n", CMD_CONN_REQ_FROM, 
				CMD_CONN_REQ_FROM);
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
			return;
		}
		
		// store socket of the other end
		memset(username, 0, strlen(username));
		buffer.copy(username, buffer.length() - user_pos, user_pos);
		dprintf("[CLIENT]connected with %s\n", username);
		client->insert_in_connected_users(username, sockfd);
		
		sprintf(username, "%s %s", CONNECTED_MSG, client->get_username().c_str());
		
		// Tell the other end connection is established.
		assert(send(sockfd, username, strlen(username) + 1, 0) >= 0);
		
		return;
	}

	if (buffer.find(CONNECTED_MSG) == 0) {
		int user_pos 	= buffer.find(" ") + 1;
		string username = buffer.substr(user_pos);
		dprintf("[CLIENT]just start sending file to %s\n", username.c_str());
		client->insert_in_connected_users(username, sockfd);
		return;
	}
	
	if (buffer.find(ERR_MSG) == 0) {
		dprintf("[CLIENT]cannot start sending file :( error encountered\n");
		return;
	}
	
}

/**
 * Initialize server
 */
int run_server(char * server_ip, int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, socket_server, fdmax, n, newsockfd;
	string ip;
	fd_set read_fds, tmp_fds;
	FD_ZERO(&tmp_fds);

	// Init localhost server (communication with other clients)
	init_server(client_port, sockfd, fdmax, &read_fds);

	dprintf("Client has port %i\n", client_port);

	// Connect to main server and send port of this client (port which be server port for the other clients who want
	// to communicate directly with this server)
	connect_to_server(server_ip, server_port, socket_server, fdmax, &read_fds);
	sprintf(buffer, "%s %i", INFO_CLIENT_PORT, client_port);
	dprintf("sending port %i to server\n", client_port);
	assert(send(socket_server, buffer, strlen(buffer) + 1, 0) >= 0);

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
					new_connection(sockfd, fdmax, &read_fds, ip, newsockfd);
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
					if (n <= 0) {
						dprintf("server hung up\n");
						end_connection(i, &read_fds);
						goto exit_label;
						break;
					}
					dprintf("[CLIENT]received from SERVER: %s\n", buffer);
					process_server_msg(string(buffer), fdmax, &read_fds);
					continue;
				}

				// Received data from another client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					end_connection(i, &read_fds);
					client->remove_from_connected_users(i);
				} else {
					dprintf("received from client: %s\n", buffer);
					client_command(string(buffer), i);
				}
			}
		}
	}
exit_label:
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
