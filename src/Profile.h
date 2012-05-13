/*
 * Profile.h
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail
 */

#ifndef PROFILE_H_
#define PROFILE_H_

class Profile {
	std::string name;
	std::string surname;
	std::string phone;
	std::string email;
	std::string hobbies;

public:
	Profile();
	Profile(std::string, std::string, std::string,
		std::string, std::string);
	virtual ~Profile();

	void set_name(std::string name);
	std::string get_name();

	void set_surname(std::string surname);
	std::string get_surname();

	void set_phone(std::string phone);
	std::string get_phone();

	void set_email(std::string email);
	std::string get_email();

	void set_hobbies(std::string hobbies);
	std::string get_hobbies();
};

#endif /* PROFILE_H_ */
