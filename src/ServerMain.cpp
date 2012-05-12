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
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "Server.h"

// Defines
#define BANNER			"OK"

using namespace std;
Server *server;

static void stdin_command();
static void announce_friends_online_status(int sockfd);
static void client_command(string line, int sockfd, Server *&server);

/**
 * Execute command received from STDIN
 */
static void stdin_command() {
	string line;
	getline(cin, line);

	// Tratarea comenzii de la tastatura TODO
		if (line.find(EXIT_MSG) == 0) {
			//TODO end all connections
			exit(EXIT_SUCCESS);
		}
}

static void announce_friends_online_status(int sockfd) {
	map<int, ClientInfo*> map_fdcl;
	map<string, int> map_clfd;
	map<int, ClientInfo*>::iterator it_fdcl;
	map<string, int>::iterator it_clfd;
	string friends = "", friend_name;
	stringstream announcement;
	int friend_sockfd, pos, next_pos, len;
	
	map_fdcl = server->get_sockfd_to_clients();
	it_fdcl = map_fdcl.find(sockfd);
	assert(it_fdcl != map_fdcl.end());
	
	ClientInfo *client = it_fdcl->second;
	
	friends = string(server->get_list_of_friends(client->get_username()));
	
	if (friends.compare("") == 0)
		return;
	
	for (pos = 0, next_pos = friends.find(","), len = friends.length(); pos != string::npos && pos < len; 
		pos = next_pos + 1, next_pos = friends.find(",", pos)) 
	{
		if (next_pos == string::npos) {
			friend_name = friends.substr(pos, len);
			next_pos = len;
		}
		else
			friend_name = friends.substr(pos, next_pos);
		dprintf("[SERVER]%s is friend of %s\n", friend_name.c_str(), client->get_username().c_str());

		map_clfd = server->get_clients_to_sockfd();
		it_clfd = map_clfd.find(friend_name);
		if (it_clfd == map_clfd.end())
			//friend is not online, do nothing
			continue;

		friend_sockfd = it_clfd->second;
		
		//send to friend_socket (client.username, client.ip, client.port);
		announcement << "friend_online " << client->get_username() << " " << client->get_ip() << " " <<
				client->get_port();
		assert(send(friend_sockfd, announcement.str().c_str(), announcement.str().length() + 1, 0) >= 0);
	}
}

static void client_command(string line, int sockfd, Server *&server) {

	if (line.find(CMD_REGISTER) == 0) {
		int user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1,
			email_pos = line.find(" ", pass_pos) + 1;
		server->register_client(sockfd,
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos, email_pos - 1 - pass_pos),
				line.substr(email_pos));
		return;
	}

	if (line.find(CMD_AUTH) == 0) {
		int user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1,
			ip_pos = line.find(" ", pass_pos) + 1;
			
		ClientInfo *ci = server->get_client_info(sockfd);
		
		server->authentication(sockfd,
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos),
				ci->get_ip(),
				ci->get_port());

		announce_friends_online_status(sockfd);
		return;
	}

}

/**
 * Initialize server
 */
int run_server(int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, fdmax, n, newsockfd, newport;
	string ip;
	fd_set read_fds, tmp_fds;
	FD_ZERO(&tmp_fds);

	// Init server
	init_server(server_port, sockfd, fdmax, &read_fds);

	server = new Server();

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
					//store information of client connected on newsockfd
					server->insert_in_sockfd_to_clients(newsockfd, new ClientInfo(ip, newport));
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
