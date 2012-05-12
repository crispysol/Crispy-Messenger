/*
 * client.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: andreea
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

void Client::insert_in_sockfd_to_clients(int key, ClientInfo * ci) {
	sockfd_to_clients.insert(pair<int, ClientInfo*> (key, ci));
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
			user->set_username(users[i]["name"].asString());
			user->set_state_from_string(users[i]["state"].asString());
			user->set_status(users[i]["status"].asString());
		}
	}

	// Get offline messages TODO

	cout << root.toStyledString(); // TODO delete

	return true;
}

