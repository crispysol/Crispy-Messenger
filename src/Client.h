/*
 * ClientImpl.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail, andreea
 */

#ifndef CLIENTIMPL_H_
#define CLIENTIMPL_H_

#include <string>
#include <vector>
#include <map>

#include "ServerFunctions.h"
#include "Profile.h"
#include "User.h"
#include "jsoncpp/json.h"

class Client : public User {
	// Map <group, list of friends>
	std::map <std::string, std::vector <User *> > groups;
	// Map <user, offline message>
	std::map <std::string, std::string> offline_messages;
	// Server socket
	int server_socket;
	// Map <sockfd, username>
	std::map <std::string, int> connected_users;

public:
	Client(int server_socket);
	virtual ~Client();

	// Getter
	std::map <std::string, std::vector <User *> > & get_groups();
	std::map <std::string, std::string> & get_offline_messages();
	int get_server_socket();
	std::string get_ip();
	int get_port();
	
	void insert_in_connected_users(std::string username, int sockfd);
	void remove_from_connected_users(int sockfd);

	/**
	 * Communication client-server
	 */
	bool register_client(std::string username, std::string pass, std::string email);
	bool authentication(std::string username, std::string pass);

	Profile get_profile(std::string username);
	bool update_profile(std::string name, std::string surname,
			    std::string phone, std::string hobbies);

	// Add/Remove/Search a user
	bool add_user(std::string username);
	bool remove_user(std::string username);
	std::string search_user(Profile profile);

	// Add/Remove/Edit a group
	bool add_group(std::string group);
	bool remove_group(std::string group);
	bool move_user_to_group(std::string username, std::string group);

	// Status/State
	bool send_status(std::string status);
	bool send_state(State state);

	// Refresh a user (message sent by server when another user changes status/state)
	bool refresh_user(User user);

	/**
	 * Asks for port and ip of user <username>.
	 */
	void connect_with_user_req(std::string username);
	/**
	 * Returns socket file descriptor of the other user or -1 on error.
	 */
	int connect_with_user_res(char* response, int & fdmax, fd_set * read_fds);

	// Receive friend list
	bool receive_friend_list(Json::Value & root);

	/**
	 * Communication client-client
	 */
	bool send_message(int sockfd, std::string message);
	//TODO not needed recv_message(int sockfd, std::string message); not needed
};



#endif /* CLIENTIMPL_H_ */
