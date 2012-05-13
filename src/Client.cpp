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

void Client::remove_from_connected_users(int sockfd) {
	//TODO
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

bool Client::receive_friend_list(Json::Value & root) {
	// Synchronize server / client
	assert(send(server_socket, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);

	// Receive friends
	char buffer[BUFFER_LENGTH];
	int rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);

	// Parse json
	Json::Reader reader;
	if (!reader.parse(buffer, root, false)) {
		cout << "Failed to parse JSON" << endl << reader.getFormatedErrorMessages()<< endl;
		return false;
	}

	// Erase old map
	this->groups.clear();

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

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
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

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

	// Get offline messages TODO

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

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

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

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

	return true;
}

/**
 * Requests profile information from server.
 *
 * Liviu
 */
Profile Client::get_profile(std::string username) {
	int rc;
	char buff[BUFFER_LENGTH];

	//send username to server_socket
	sprintf(buff, "%s %s", CMD_GET_PROFILE, username.c_str());
	assert(send(server_socket, buff, strlen(buff) + 1, 0) >= 0);

	memset(buff, 0, BUFFER_LENGTH);

	//receive response form server_socket
	rc = recv(server_socket, buff, sizeof(buff), 0);
	assert(rc >= 0);
	dprintf("Received from server: %s\n", buff);

	string buf = buff;

	int	name_pos = buf.find(" "),
		sname_pos = buf.find(" ", name_pos) + 1,
		phone_pos = buf.find(" ", sname_pos) + 1,
		email_pos = buf.find(" ", phone_pos) + 1,
		hobb_pos = buf.find(" ", email_pos) + 1;
	/*
	//TODO: Remove DEBUG
	cout	<< "Name: |" << buf.substr(0, name_pos) << "|\n"
		<< "Surname: |" << buf.substr(pos2, pos3 - pos2 - 1)  << "|\n"
		<< "Phone: |" << buf.substr(pos3, pos4 - pos3 - 1) << "|\n"
		<< "email: |" << buf.substr(pos4, pos5 - pos4 - 1) << "|\n"
		<< "Hobbies: |" << buf.substr(pos5) << "|\n";
	*/
	
	return Profile( buf.substr(0, name_pos),
			buf.substr(sname_pos, phone_pos - sname_pos - 1),
			buf.substr(phone_pos, email_pos - phone_pos - 1),
			buf.substr(email_pos, hobb_pos - email_pos - 1),
			buf.substr(hobb_pos) );
}

/**
 * Updates the client's profile information.
 * NULL fields will not be updated.
 *
 * Liviu
 */
bool Client::update_profile(std::string name, std::string surname, std::string phone,
		std::string email, std::string hobbies) {
	return true;
}



bool Client::remove_group(std::string group) {
	int rc;
	char buffer[BUFFER_LENGTH];

	//send group to delete,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_DEL_GROUP, group.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

	return true;

}


bool Client::move_user_to_group(std::string username, std::string group){
	int rc;
	char buffer[BUFFER_LENGTH];

	//send group to delete,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s %s", CMD_MV_USER, username.c_str(), group.c_str());
	dprintf("%s\n",msg);
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

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
int Client::connect_with_user_res(string response, int & fdmax, fd_set * read_fds) {
	int rc, newsocket, port;
	char ip[16];
	char username[BUFFER_LENGTH];
	
	//receive port and ip from server	
	if (response.compare(ERR_MSG) == 0)
		return -1;
	
	vector<string> tokens;
	tokenize(response, tokens, " ");
	vector<string>::iterator tok = tokens.begin(), tok_end = tokens.end();
	assert(tok != tok_end);
	tok++;
	assert(tok != tok_end);
	strcpy(ip, (*tok).c_str());
	tok++;
	assert(tok != tok_end);
	port = atoi((*tok).c_str());
	tok++;
	assert(tok != tok_end);
	strcpy(username, (*tok).c_str());
	
	dprintf("received from server: %s, %s, %i\n", username, ip, port);
	
	//connect_to_server(consider the other client the server)
	connect_to_server(ip, port, newsocket, fdmax, read_fds);
	dprintf("connected to %s\n", username);
	insert_in_connected_users(string(username), newsocket);
	
	//return socket from connect_to server
	return newsocket;

}
