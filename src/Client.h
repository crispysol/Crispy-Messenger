/*
 * ClientImpl.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail
 */

#ifndef CLIENTIMPL_H_
#define CLIENTIMPL_H_

#include <string>
#include <vector>
#include <map>

#include "Profile.h"

enum State {AVAILABLE, AWAY, BUSY, OFFLINE};

class Client : public User {
	// Map <group, list of friends>
	std::map <std::string, std::vector <Client> > groups;
	// Map <user, offline message>
	std::map <std::string, std::string> offline_messages;

public:
	Client();
	virtual ~Client();

	std::map <std::string, std::vector <Client> > get_groups();
	std::map <std::string, std::string> get_offline_messages();

	// Communication client-server
	bool register_client(std::string username, std::string pass, std::string email);
	bool authentication(std::string username, std::string pass);

	Profile get_profile(std::string username);
	bool update_profile(std::string name, std::string surname, std::string phone,
		std::string email, std::string hobbies);

	bool add_user(std::string username);
	bool remove_user(std::string username);
	std::string search_user(Profile profile);

	bool add_group(std::string group);
	bool remove_group(std::string group);
	bool move_user_to_group(std::string username, std::string group);

	bool send_status(std::string status);
	bool send_state(State state);

	// Communication server-client
	bool update(Client client);

	// Communication client-client
	bool connect_with_user(std::string username);
	bool send_message(std::string message);
	std::string recv_message();
};



#endif /* CLIENTIMPL_H_ */
