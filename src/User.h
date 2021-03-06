/*
 * User.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail
 */

#ifndef USER_H_
#define USER_H_

#include "Profile.h"

enum State {AVAILABLE, AWAY, BUSY, OFFLINE};

class User {
	Profile profile;
	std::string username;
	std::string status;
	State state;

public:
	User();
	virtual ~User();

	// Getter
	virtual Profile & get_profile();

	// Setter-Getter
	void set_username(std::string username);
	std::string get_username();

	void set_status(std::string status);
	std::string get_status();

	void set_state(State state);
	State get_state();

	void set_state_from_string(std::string state);
	std::string get_state_as_string();
};

#endif /* USER_H_ */
