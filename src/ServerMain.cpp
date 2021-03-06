/*
 * server.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: mihail, andreea, radu
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
static void stdin_command(fd_set * read_fds) {
	string line;
	getline(cin, line);

	// Tratarea comenzii de la tastatura
	if (line.find(EXIT_MSG) == 0) {
		//end all connections
		map<int, ClientInfo*> clients 		= server->get_sockfd_to_clients();
		map<int, ClientInfo*>::iterator it 	= clients.begin(), 
						it_end	= clients.end();
		for (; it != it_end; it++)
			end_connection(it->first, read_fds);	
					
		exit(EXIT_SUCCESS);
	}
}

static void announce_friends_online_status(int sockfd) {
	
	map<int, ClientInfo*> map_fdcl;
	map<string, int> map_clfd;
	//iterator for socketfd_to_clients map
	map<int, ClientInfo*>::iterator it_fdcl;
	//ierator for clients_to_sockfd map
	map<string, int>::iterator it_clfd;
	map<string, string> groups;
	//iterator for friends within groups
	map<string, string>::iterator it_f;
	string friend_name, friends;
	stringstream announcement;
	int friend_sockfd, pos, next_pos, len;
	char buff[BUFFER_LENGTH];
	
	map_fdcl = server->get_sockfd_to_clients();
	it_fdcl = map_fdcl.find(sockfd);
	assert(it_fdcl != map_fdcl.end());
	
	ClientInfo *client = it_fdcl->second;
	
	groups = server->get_list_of_friends(client->get_username());
	it_f = groups.begin();
	
	for (; it_f != groups.end(); it_f++) {
		friends = it_f->second;
		next_pos = friends.find(",");
		len = friends.length();
		for (pos = 0; pos != string::npos && pos < len; pos = next_pos + 1, next_pos = friends.find(",", pos)) {
			if (next_pos == string::npos) {
				friend_name = friends.substr(pos, len);
				next_pos = len;
			}
			else
				friend_name = friends.substr(pos, next_pos);
			dprintf("[SERVER]%s is friend of %s\n", friend_name.c_str(), client->get_username().c_str());

			map_clfd = server->get_clients_to_sockfd();
			it_clfd = map_clfd.find(friend_name);
			if (it_clfd == map_clfd.end()) {
				//friend is not online, do nothing
				cout << friend_name << " is not online!\n";
				continue;
			}

			friend_sockfd = it_clfd->second;
		
			//send to friend_socket (client.username, client.ip, client.port);
			announcement << FRIEND_IS_ONLINE << " " << client->get_username() << " " << client->get_ip() << " " <<
					client->get_port();

			cout << "Sending " << announcement << endl;
			assert(send(friend_sockfd, announcement.str().c_str(), announcement.str().length() + 1, 0) >= 0);

			server->send_friends_list(friend_sockfd, friend_name);
		}
	}
}

static void client_command(string line, int sockfd, Server *&server) {

	if (line.find(CMD_REGISTER) == 0) {
		int 	user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1,
			email_pos = line.find(" ", pass_pos) + 1;
		server->register_client(sockfd,
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos, email_pos - 1 - pass_pos),
				line.substr(email_pos));
		return;
	}
	
	ClientInfo *ci = server->get_clientInfo_by_sockfd(sockfd);
	
	
	if (line.find(INFO_CLIENT_PORT) == 0) {
		int	port_pos = line.find(" ") + 1, port;
		char	port_ch[6];
		assert(line.copy(port_ch, line.length() - port_pos, port_pos) > 0);
		port = atoi(port_ch);
		dprintf("[SERVER]received port %i\n", port);
		assert(ci != NULL);
		ci->set_port(port);
		
		return;
	}
	
	if (line.find(CMD_AUTH) == 0) {
		int 	user_pos = line.find(" ") + 1,
			pass_pos = line.find(" ", user_pos) + 1,
			ip_pos = line.find(" ", pass_pos) + 1;
		
		server->authentication(sockfd,
				line.substr(user_pos, pass_pos -1 - user_pos),
				line.substr(pass_pos),
				ci->get_ip(),
				ci->get_port());

		announce_friends_online_status(sockfd);
		return;
	}

	if (ci->get_username().empty()) {
		dprintf("[SERVER]Can not execute '%s'; user is not logged in\n", line.c_str());
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
		return;
	}
	if(line.find(CMD_ADD_USER) == 0){
	
		dprintf("sending add_user command\n");
		int user_pos = line.find(" ") + 1;
		server->add_user(sockfd, line.substr(user_pos));
		return;
	}

	if (line.find(CMD_REMOVE_USER) == 0) {
		int user_pos = line.find(" ") + 1;
		server->remove_user(sockfd, line.substr(user_pos));
		return;
	}
	
	if (line.find(CMD_ADD_GROUP) == 0) {
		int group_pos = line.find(" ") + 1;
		
		server->add_group(sockfd, line.substr(group_pos), ci->get_username());
		return;
	}

	if(line.find(CMD_DEL_GROUP) == 0) {
		int group_pos = line.find(" ") + 1;
		server->remove_group(sockfd,line.substr(group_pos));
		return;
	}

	if(line.find(CMD_MV_USER) == 0) {
		int user_pos= line.find(" ") + 1,
			group_pos = line.find(" ",user_pos) + 1;
			
		server-> move_user_to_group(	sockfd,
						line.substr(user_pos, group_pos-1 - user_pos),
						line.substr(group_pos)
						);
		return;
	}
	 
	if (line.find(CMD_CONN_CLIENT_TO_CLIENT_REQ) == 0) {
		int username_pos = line.find(" ") + 1;
		dprintf("[SERVER] processing %s request from %s to %s\n",
			CMD_CONN_CLIENT_TO_CLIENT_REQ, ci->get_username().c_str(),
			line.substr(username_pos).c_str());
		server->send_user_ip(sockfd, line.substr(username_pos));

		return;
	}

	if (line.find(CMD_GET_PROFILE) == 0) {
		int user_pos = line.find(" ") + 1;

		server->send_profile(sockfd, line.substr(user_pos));
		return;
	}

	if (line.find(CMD_UPDATE_PROFILE) == 0) {
		int	name_pos = line.find(" ") + 1,
			sname_pos = line.find(" ", name_pos) + 1,
			phone_pos = line.find(" ", sname_pos) + 1,
			hobb_pos = line.find(" ", phone_pos) + 1;

		server->update_profile(
			sockfd,
			line.substr(name_pos, sname_pos - name_pos - 1),
			line.substr(sname_pos, phone_pos - sname_pos - 1),
			line.substr(phone_pos, hobb_pos - phone_pos - 1),
			line.substr(hobb_pos));
		return;
	}
	
	if (line.find(CMD_SEND_MSG) == 0) {
		int 	user_src_pos = line.find(" ") + 1,
			user_dst_pos = line.find(" ", user_src_pos) + 1,
			msg_pos = line.find(" ", user_dst_pos) + 1;
			server->send_msg_from_user_to_user(sockfd, 
				line.substr(user_src_pos, user_dst_pos - user_src_pos - 1),
				line.substr(user_dst_pos, msg_pos - user_dst_pos - 1),
				line.substr(msg_pos));
		return;
	}

	
	if (line.find(INFO_CLIENT_PORT) == 0) {
		int	port_pos = line.find(" ") + 1, port;
		char	port_ch[6];
		assert(line.copy(port_ch, line.length() - port_pos, port_pos) > 0);
		port = atoi(port_ch);
		dprintf("received port %i\n", port);
		ClientInfo *ci = server->get_clientInfo_by_sockfd(sockfd);
		assert(ci != NULL);
		ci->set_port(port);
		
		return;
	}

	if (line.find(CMD_SET_STATE) == 0) {
		int state_pos = line.find(" ") + 1;

		server->set_state(sockfd, line.substr(state_pos));
		announce_friends_online_status(sockfd);
		return;
	}
	
	if (line.find(CMD_SET_STATUS) == 0) {
		int status_pos = line.find(" ") + 1;

		server->set_status(sockfd, line.substr(status_pos));
		announce_friends_online_status(sockfd);
		return;
	}
	
	if (line.find(CMD_SEARCH_USER) == 0) {
		int 	name_pos = line.find(" ") + 1,
			surname_pos = line.find(" ", name_pos) + 1,
			phone_pos = line.find(" ", surname_pos) + 1,
			email_pos = line.find(" ", phone_pos) + 1;
		server->search_user(sockfd,
				line.substr(name_pos, surname_pos -1 - name_pos),
				line.substr(surname_pos, phone_pos - 1 - surname_pos),
				line.substr(phone_pos, email_pos - 1 - phone_pos),
				line.substr(email_pos));
		
		return;
	}

}

/**
 * Initialize server
 */
int run_server(int server_port) {
	char buffer[BUFFER_LENGTH];
	int sockfd, fdmax, n, newsockfd;
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
					new_connection(sockfd, fdmax, &read_fds, ip, newsockfd);
					//store information of client connected on newsockfd
					server->insert_in_sockfd_to_clients(newsockfd, new ClientInfo(ip));
					continue;
				}

				// Execute command from STDIN
				if (i == STDIN_FILENO) {
					stdin_command(&read_fds);
					continue;
				}

				// Received data from client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					//assert(n == 0); TODO
					//set client status offline
					server->set_offline(i); 
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
