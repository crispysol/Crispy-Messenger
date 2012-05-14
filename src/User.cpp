/*
 * User.cpp
 *
 *  Created on: May 12, 2012
 *      Author: mihailc
 */

#include <string>
#include "User.h"

User::User() {
	this->status = "";
	this->state = OFFLINE;
}

User::~User() {}

Profile & User::get_profile() {
	return profile;
}

void User::set_username(std::string username) {
	this->username = username;
}
std::string User::get_username() {
	return username;
}

void User::set_status(std::string status) {
	this->status = status;
}
std::string User::get_status() {
	return status;
}

void User::set_state(State state) {
	this->state = state;
}
State User::get_state() {
	return state;
}

void User::set_state_from_string(std::string state) {
	if (state == "available") {
		this->state = AVAILABLE;
	} else if (state == "away") {
		this->state = AWAY;
	} else if (state == "busy") {
		this->state = BUSY;
	} else {
		this->state = OFFLINE;
	}
}
std::string User::get_state_as_string() {
	switch (state) {
		case AVAILABLE:
			return "available";
		case AWAY:
			return "away";
		case BUSY:
			return "busy";
		default:
			return "offline";
	}
}
