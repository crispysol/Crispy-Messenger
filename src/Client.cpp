/*
 * client.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: andreea, mihail, radu
 */

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerFunctions.h"
#include "Client.h"
#include "jsoncpp/json.h"

using namespace std;

Client::Client(int server_socket) : server_socket(server_socket)
{}

Client::~Client()
{}

std::map <std::string, std::vector <User *> > & Client::get_groups() {
	return groups;
}

int Client::get_server_socket() {
	return server_socket;
}

void Client::insert_in_connected_users(std::string username, int sockfd) {
	connected_users.insert(pair<string, int> (username, sockfd));
}

bool Client::register_client(std::string username, std::string pass, std::string email) {
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username, pass, email to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s %s %s", CMD_REGISTER, username.c_str(), pass.c_str(), email.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, USEDUSER_ERR) == 0 || strcmp(buffer, USEDEMAIL_ERR) == 0 ||
			strcmp(buffer, ERR_MSG) == 0)
		return false;

	return true;
}


bool Client::authentication(std::string username, std::string pass) {
	int rc;
	char buffer[BUFFER_LENGTH];

	// send username, pass, ip, port to server_socket
	char msg[BUFFER_LENGTH];
	
	sprintf(msg, "%s %s %s", CMD_AUTH, username.c_str(), pass.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;
	
	// receive friends
	rc = recv(server_socket, buffer, sizeof(buffer), 0);

	// Parse json
	Json::Value root;
	Json::Reader reader;
	if (!reader.parse(buffer, root, false)) {
		cout  << "Failed to parse JSON"<< endl << reader.getFormatedErrorMessages()<< endl;
		return false;
	}

	// Get groups
	const Json::Value groups = root["groups"];
	for (int i = 0, imax = groups.size(); i < imax; i++) {
		string group_name = groups[i]["name"].asString();
		vector<User *> vect;
		const Json::Value users = groups[i]["users"];
		for (int j = 0, jmax = users.size(); j < jmax; j++) {
			User * user = new User();
			user->set_username(users[j]["name"].asString());
			user->set_state_from_string(users[j]["state"].asString());
			user->set_status(users[j]["status"].asString());
			vect.push_back(user);
		}
		this->groups.insert(pair <string, vector <User *> > (group_name, vect));
	}

	// Get offline messages TODO

	cout << root.toStyledString(); // TODO delete

	return true;
}

bool Client::add_user(std::string username)
{
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_ADD_USER, username.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0 || strcmp(buffer, USER_ALREADY_IN_LIST) == 0)
		return false;

	return true;
}

bool Client::remove_user(std::string username) {
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_REMOVE_USER, username.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;

	return true;
}

bool Client::add_group(std::string group) {
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_ADD_GROUP, group.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;

	return true;
}

/**
 * Asks for port and ip of user <username>.
 */
void Client::connect_with_user_req(std::string username) {
	char buffer[BUFFER_LENGTH];

	//send request to server for (ip, port) of user <username>
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_CONN_CLIENT_TO_CLIENT_REQ, username.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);
}

/**
 * Returns socket file descriptor of the other user or -1 on error.
 */
int Client::connect_with_user_res(char* response, int & fdmax, fd_set * read_fds) {
	int rc, newsocket, port;
	char ip[16];
	char buffer[BUFFER_LENGTH];
	
	//receive port and ip from server	
	if (response == NULL || strmcp(response, ERR_MSG) == 0)
		return -1;
	
	sscanf(response, "%s: %s %i", buffer, ip, port);
	dprintf("received from server: %s %s %i\n", buffer, ip, port);
	
	//connect_to_server(consider the other client the server)
	connect_to_server(ip, port, newsocket, fdmax, read_fds);
	insert_in_connected_users(username, newsockfd);
	
	//return socket from connect_to server
	return newsockfd;
}
