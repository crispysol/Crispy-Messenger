/*
 * client.cpp
 *
 *  Created on: Apr 8, 2012
 *      Author: andreea, mihail, radu, liviu
 */

#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cstring>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/sendfile.h>

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

/**
 * @return socket descriptor for user <username> or -1 if this client is not connected with user <username>
 */
int Client::get_socket_of_connected_user(std::string username) {
	map<string, int>::iterator it = connected_users.find(username);
	
	if ( it == connected_users.end())
		return -1;
	return it->second;
}

void Client::remove_from_connected_users(int sockfd) {
	map<string, int>::iterator it = connected_users.begin(), it_end = connected_users.end();
	string username;
	
	// Determine user who hung up
	for(; it != it_end && it->second != sockfd; it++);
	
	if (it == it_end)
		return;
		
	username = it->first;
	dprintf("[CLIENT] %s hung up\n", username.c_str());
	
	//Remove user from connected_users
	connected_users.erase(it);
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
		dprintf("Failed to parse JSON\n%s\n", reader.getFormatedErrorMessages().c_str());
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
	if (rc == 0 || strcmp(buffer, SUCCESS_MSG))
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
	if (rc == 0 || strcmp(buffer, SUCCESS_MSG))
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
	if (rc == 0 || strcmp(buffer, SUCCESS_MSG))
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
bool Client::update_profile(std::string name, std::string surname,
			    std::string phone, std::string hobbies) {
	
	char buff[BUFFER_LENGTH];

	sprintf(buff, "%s %s %s %s %s", CMD_UPDATE_PROFILE, name.c_str(), surname.c_str(),
					phone.c_str(), hobbies.c_str());
	assert(send(server_socket, buff, strlen(buff)+1, 0) >= 0);
	
	memset(buff, 0, BUFFER_LENGTH);

	//receive response form server_socket
	int rc = recv(server_socket, buff, sizeof(buff), 0);
	assert(rc >= 0);
	dprintf("Received from server: %s\n", buff);

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
	if (rc == 0 || strcmp(buffer, SUCCESS_MSG))
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
	if (rc == 0 || strcmp(buffer, SUCCESS_MSG))
		return false;

	// Receive friends
	Json::Value root;
	if (!receive_friend_list(root)) {
		return false;
	}

	return true;

}
/**
 * Asks for port and ip of user <username> if they are not already connected.
 */
void Client::connect_with_user_req(std::string username) {

	if (get_socket_of_connected_user(username) != -1) {
		dprintf("[CLIENT] already connected with %s\n", username.c_str());
		return;
	}

	//send request to server for (ip, port) of user <username>
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_CONN_CLIENT_TO_CLIENT_REQ, username.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);
}

/**
 * Returns socket file descriptor of the other user or -1 on error.
 * @param response "CMD_..._RES ip port username"
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
	
	//return socket from connect_to server
	return newsocket;

}


/**
 * Send message to user <username_dst> through server.
 */
bool Client::send_message(string username_dst, string message) {
	char buffer[BUFFER_LENGTH];
	
	/* send_msg source destination message */
	sprintf(buffer, "%s %s %s %s", CMD_SEND_MSG, get_username().c_str(), username_dst.c_str(), message.c_str());
	dprintf("sendinf %s to server\n", buffer);
	
	assert(send(server_socket, buffer, strlen(buffer) + 1, 0) >= 0);
	
	return true;	
}


bool Client::send_status(std::string status)
{
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_SET_STATUS, status.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);
	
	//receive response from server_socket
	int rc = recv(server_socket, msg, sizeof(msg), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", msg);
	if (rc == 0 || strcmp(msg, SUCCESS_MSG))
		return false;

	return true;
}	

bool Client::send_state(std::string state)
{
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_SET_STATE, state.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);
	
	//receive response from server_socket
	int rc = recv(server_socket, msg, sizeof(msg), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", msg);
	if (rc == 0 || strcmp(msg, SUCCESS_MSG))
		return false;

	return true;
}	


std::string Client::search_user(std::string profile){

	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_SEARCH_USER, profile.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);
	
	//receive response from server_socket
	int rc = recv(server_socket, msg, sizeof(msg), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", msg);
	if (rc == 0 || strcmp(msg, ERR_MSG)==0 ||strcmp(msg, NO_USER_FOUND)==0)
		return NO_USER_FOUND;

	return msg;
}

/**
 * Send file <filename> to user <username> in three steps: first send a header with file's name and size, wait for 
 * confirmation and then send the file.
 */
bool Client::send_file(string filename, string username) {
	int fd_in, sockfd, optval;
	char header[BUFFER_LENGTH];
	struct stat buf;
	
	dprintf("[SEND FILE] Processing send_file %s to %s\n", filename.c_str(), username.c_str());
	sockfd = get_socket_of_connected_user(username);
	if (sockfd == -1) {
		dprintf("[SEND FILE]No socket!\n");
		return false;
	}
		
	fd_in = open(filename.c_str(), O_RDONLY);
	if (fd_in < 0) {
		dprintf("[SEND FILE]Can't open file!\n");
		return false;
	}
	assert(fstat(fd_in, &buf) == 0);
	sprintf(header, "%s %s %u ", FILE_TRANSFER, filename.c_str(), buf.st_size);
	
	/* don't send out partial frames */		
	optval = 1;	
//	assert(setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval)) == 0);
	
	// send header and then file
	assert(send(sockfd, header, strlen(header) + 1, 0) >= 0);
	assert(recv(sockfd, header, strlen(header), 0)>=0);
	assert(sendfile(sockfd, fd_in, NULL, buf.st_size) > -1); 

	/* restore send out partial frames */
	optval ^= optval;
//	assert(setsockopt(sockfd, IPPROTO_TCP, TCP_CORK, &optval, sizeof(optval)) == 0);

	close(fd_in);
		
	return true;
}
