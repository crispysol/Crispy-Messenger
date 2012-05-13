/*
 * Functions.h
 *
 *  Created on: May 5, 2011
 *      Author: cmihail, andreea, liviu, radu
 */

#ifndef SERVER_FUNCTIONS_H_
#define SERVER_FUNCTIONS_H_

#include <stdio.h>
#include <vector>

#include "User.h"

#define BUFFER_LENGTH			20000
#define SUCCESS_MSG			"OK"
#define USEDUSER_ERR			"USEDUSER"
#define USEDEMAIL_ERR			"USEDMAIL"
#define USER_ALREADY_IN_LIST 		"USER_IN_LIST"
#define ERR_MSG				"FAIL"
#define CMD_REGISTER			"register"
#define CMD_AUTH			"login"
#define CMD_GET_PROFILE               	"get_profile"
#define CMD_UPDATE_PROFILE    		"update_profile"
#define CMD_MV_USER           		"move_user_to_group"
#define CMD_ADD_USER			"add_user"
#define CMD_REMOVE_USER			"remove_user"
#define CMD_ADD_GROUP			"add_group"
#define CMD_DEL_GROUP			"del_group"
#define CMD_GET_PROFILE			"get_profile"
#define CMD_UPDATE_PROFILE		"update_profile"
#define CMD_MV_USER			"move_user_to_group"
#define CMD_CONN_CLIENT_TO_CLIENT_REQ	"connect_with_user"
#define CMD_CONN_CLIENT_TO_CLIENT_RES	"connect_with_user_res"
#define CMD_CONN_REQ_FROM		"connect_req_from"

#define EXIT_MSG			"exit"

#define SQL_DEBUG			"SQL"

#ifdef _DEBUG
# define dprintf(...) {printf(__VA_ARGS__); fflush(stdout);}
#else
# define dprintf(...) do {} while(0)
#endif

class ClientInfo : public User {
	std::string ip;
	int port;

public:
	ClientInfo(std::string ip, int port);
	virtual ~ClientInfo();

	void set_ip(std::string ip);
	std::string get_ip();

	void set_port(int port);
	int get_port();
};

/**
 * Receive a new connection and add it to read_fds
 */
void new_connection(int sockfd, int & fdmax, fd_set * read_fds, std::string &ip, int &newsockfd, int &newport);

/**
 * End a connection and remove it from read_fds
 */
void end_connection(int sockfd, fd_set * read_fds);

/**
 * Init server
 */
void init_server(int & server_port, int & sockfd, int & fdmax, fd_set * read_fds);

/**
 * Connect to a server (only for client)
 */
void connect_to_server(char * server_ip, int server_port,  int & socket_server, int & fdmax, fd_set * read_fds);

/**
 * Tokenizer
 */
void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters);


#endif //define SERVER_FUNCTIONS_H_
