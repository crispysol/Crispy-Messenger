/*
 * Server.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail, andreea
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "User.h"

#define DATABASE_HOST 	"localhost"
#define DATABASE_USER 	"crispy"
#define DATABASE_PASS 	"M3$$enger"
#define DATABASE_NAME 	"CrispyDatabase"
#define PASS_FIELD	"password_hash"

//Groups table headers
#define GROUPS_T_ID		"id"
#define GROUPS_T_ID_USER_FK	"id_user"
#define GROUPS_T_NAME		"name"
#define GROUPS_T_FRIENDS_LIST	"friends_list"
#define GROUP_DEFAULT_NAME	"friends"

class ClientInfo: public User {
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

class Server {
	// Map <sockfd, client info>
	std::map <int, ClientInfo> clients;
	// Database (initialized in constructor) TODO
	sql::Connection	*con;

public:
	Server();
	virtual ~Server();

	//TODO bool process_protocol(std::string username, std::string pass, std::string email); TODO in main and for client too
	/**
	 * Communication client-server
	 */
	bool register_client(int sockfd, std::string username, std::string pass, std::string email);
	bool authentication(int sockfd, std::string username, std::string pass, std::string ip, int port);

	bool send_profile(int sockfd, std::string username);
	bool update_profile(int sockfd, std::string name, std::string surname,
			std::string phone, std::string email, std::string hobbies);

	// Add/Remove/Search a user for a client
	bool add_user(int sockfd, std::string username);
	bool remove_user(int sockfd, std::string username);
	bool search_user(int sockfd, std::string name, std::string surname,
			std::string phone, std::string email, std::string hobbies);

	// Add/Remove/Edit a group
	bool add_group(int sockfd, std::string group);
	bool remove_group(int sockfd, std::string group);
	bool move_user_to_group(int sockfd, std::string username, std::string group);

	// Status/State (+ send message to all friends that client changed status/state)
	bool set_status(int sockfd, std::string status);
	bool set_state(int sockfd, State state);

	// Send user's ip and port
	bool send_user_ip(int sockfd, std::string username);

	// Receive message for an offline user so it can be sent at login
	bool recv_offline_message(int sockfd, std::string username, std::string message);
	
	//TODO
	bool forgot_password();
};

#endif /* SERVER_H_ */
