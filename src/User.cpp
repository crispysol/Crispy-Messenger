/*
 * User.cpp
 *
 *  Created on: May 12, 2012
 *      Author: mihailc
 */

#include <string>
#include "User.h"

using namespace std;

User::User() {}

User::~User() {}

Profile & User::get_profile() {
	return profile;
}

void User::set_username(string username) {
	this->username = username;
}

string User::get_username() {
	return username;
}

void User::set_status(string status) {
	this->status = status;
}

string User::get_status() {
	return status;
}

void User::set_state(State state) {
	this->state = state;
}

State User::get_state() {
	return state;
}
