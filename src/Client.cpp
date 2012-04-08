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

using namespace std;

Client::Client(int server_socket, std::string ip) : server_socket(server_socket), ip(ip)
{}

Client::~Client()
{}

std::string Client::get_ip() {
	return ip;
}

int Client::get_port() {
	return port;
}

int Client::get_server_socket() {
	return server_socket;
}

bool Client::register_client(std::string username, std::string pass, std::string email) {
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username, pass, email to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s %s %s", CMD_REGISTER, username.c_str(), pass.c_str(), email.c_str());
	assert(send(server_socket, msg, strlen(msg), 0) >= 0);

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

	//send username, pass, ip, port to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s %s %s %i", CMD_AUTH, username.c_str(), pass.c_str(), ip.c_str(), 8080);//, port);
	assert(send(server_socket, msg, strlen(msg), 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0)
		return false;
	
	return true;
}
