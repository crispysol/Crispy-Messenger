/*
 * Server.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail, andreea, radu, liviu
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "ServerFunctions.h"

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

class Server {
	// Map <sockfd, client info>
	std::map <int, ClientInfo*> sockfd_to_clients;
	// Map <username, sockfd>
	std::map <std::string, int> clients_to_sockfd;
	// Database (initialized in constructor)
	sql::Connection	*con;

public:
	Server();
	virtual ~Server();
	
	std::map <int, ClientInfo*> & get_sockfd_to_clients();
	std::map <std::string, int> & get_clients_to_sockfd();
	
	void insert_in_sockfd_to_clients(int key, ClientInfo * ci);
	void insert_in_clients_to_sockfd(std::string username, int sockfd);
	/* Return value in sockfd_to_clients with key <sockfd> */
	ClientInfo * get_clientInfo_by_sockfd(int sockfd);
	/* Return connection info of client <username>  */
	int get_clientInfo_by_username(std::string username); 

	//TODO bool process_protocol(std::string username, std::string pass, std::string email); TODO in main and for client too
	/**
	 * Communication client-server
	 */
	bool register_client(int sockfd, std::string username, std::string pass, std::string email);
	bool authentication(int sockfd, std::string username, std::string pass, std::string ip, int port);

	bool send_profile(int sockfd, std::string username);
	bool update_profile(int sockfd, std::string name, std::string surname,
			std::string phone, std::string hobbies);

	// Add/Remove/Search a user for a client
	bool add_user(int sockfd, std::string username);
	bool remove_user(int sockfd, std::string username);
	bool search_user(int sockfd, std::string name, std::string surname,
			std::string phone, std::string email);
	bool rmv_user(int  sockfd, std::string username );
	// Add/Remove/Edit a group
	bool add_group(int sockfd, std::string group, std::string username);
	bool remove_group(int sockfd, std::string group);
	bool move_user_to_group(int sockfd, std::string username, std::string group);

	// Status/State (+ send message to all friends that client changed status/state)
	bool set_status(int sockfd, std::string status);
	bool set_state(int sockfd, std::string state);

	// Send user's ip and port in the following format "connect_with_user_res: ip port" 
	bool send_user_ip(int sockfd, std::string username);

	// Receive message for an offline user so it can be sent at login
	bool recv_offline_message(int sockfd, std::string username, std::string message);
	
	//TODO
	bool forgot_password();
	//set user offline
	bool set_offline(int sockfd);
	std::map<std::string, std::string> get_list_of_friends(std::string username);
	
	bool send_msg_from_user_to_user(int sockfd, std::string user_src, std::string user_dst, std::string msg);

	// Send friend list
	bool send_friends_list(int sockfd, std::string username);
};

#endif /* SERVER_H_ */
