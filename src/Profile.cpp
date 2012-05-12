/*
 * Profile.cpp
 *
 *  Created on: May 12, 2012
 *      Author: mihailc
 */

#include <string>
#include "Profile.h"

Profile::Profile() {}

Profile::~Profile() {}

void Profile::set_name(std::string name) {
	this->name = name;
}
std::string Profile::get_name() {
	return name;
}

void Profile::set_surname(std::string surname) {
	this->surname = surname;
}
std::string Profile::get_surname() {
	return surname;
}

void Profile::set_phone(std::string phone) {
	this->phone = phone;
}
std::string Profile::get_phone() {
	return phone;
}

void Profile::set_email(std::string email) {
	this->email = email;
}
std::string Profile::get_email() {
	return email;
}

void Profile::set_hobbies(std::string hobbies) {
	this->hobbies = hobbies;
}
std::string Profile::get_hobbies() {
	return hobbies;
}
