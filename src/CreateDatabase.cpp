/*
 * CreateDatabase.cpp
 *
 *  Created on: Mar 11, 2012
 *      Author: mihail, andreea
 */

#include "Server.h"
#include <string>

using namespace std;


int main() {
	sql::mysql::MySQL_Driver *driver;
	sql::Connection	*con;
	sql::Statement	*stmt;

	// Connect to mysql
	driver = sql::mysql::get_mysql_driver_instance();
	con = driver->connect(DATABASE_HOST, DATABASE_USER, DATABASE_PASS);

	stmt = con->createStatement();
	// Create database
	string s = string("CREATE DATABASE IF NOT EXISTS ").append(DATABASE_NAME);
	stmt->execute(s);
	// Use database
	s = string("USE ").append(DATABASE_NAME);
	stmt->execute(s);
	// Create tables
	stmt->execute("DROP TABLE IF EXISTS users");
	s = "CREATE TABLE users ("
			"id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,"
			"username VARCHAR(30) NOT NULL,"
			"password_hash VARCHAR(32) NOT NULL,"
			"name VARCHAR(15),"
			"surname VARCHAR(15),"
			"phone VARCHAR(15),"
			"email VARCHAR(50) NOT NULL,"
			"hobbies TEXT"
			")";
	stmt->execute(s);
	stmt->execute("DROP TABLE IF EXISTS groups");
	s = string("CREATE TABLE groups (").
		append(GROUPS_T_ID).append(" INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,").
		append(GROUPS_T_ID_USER_FK).append(" INT UNSIGNED NOT NULL,").
		append(GROUPS_T_NAME).append(" VARCHAR(30) NOT NULL,").
		append(GROUPS_T_FRIENDS_LIST).append(" TEXT").
		append(")");
/*	s = "CREATE TABLE groups ("
			"id INT UNSIGNED AUTO_INCREMENT PRIMARY KEY,"
			"id_user INT UNSIGNED NOT NULL,"
			"name VARCHAR(30) NOT NULL,"
			"friends_list TEXT"
			")";
*/
	stmt->execute(s);

	delete stmt;
	delete con;
}
