/*
 * server.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: andreea
 */

#include <iostream>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <sstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <mysql_connection.h>
#include <mysql_driver.h>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/exception.h>

#include "Server.h"

using namespace std;

static sql::Statement	*stmt;


Server::Server() {
	sql::mysql::MySQL_Driver *driver;
	
	try{
		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect(DATABASE_HOST, DATABASE_USER, DATABASE_PASS);
		con->setSchema(DATABASE_NAME);
		
		stmt = con->createStatement();
	}catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		exit(EXIT_FAILURE);
	}
	
}

Server::~Server() {
	if (con)
		delete con;
	
}

map <int, ClientInfo*> Server::get_sockfd_to_clients() {
	return sockfd_to_clients;
}

map <std::string, int> Server::get_clients_to_sockfd() {
	return clients_to_sockfd;
}

void Server::insert_in_sockfd_to_clients(int key, ClientInfo * ci) {
	sockfd_to_clients.insert(pair<int, ClientInfo*> (key, ci));
}

ClientInfo * Server::get_client_info(int sockfd) {
	map<int, ClientInfo*>::iterator it_fdcl;
	
	it_fdcl = sockfd_to_clients.find(sockfd);
	assert(it_fdcl != sockfd_to_clients.end());
	
	return it_fdcl->second;
}

/* Add default group for user <username>. 
 * 
 * @return true if insert in groups table succeded, false otherwise
 */
static bool add_default_group(string username) {

	stringstream query;

	try {		
		query.flush();
		query << "INSERT INTO groups(" << GROUPS_T_ID_USER_FK << ", " <<
			GROUPS_T_NAME << ") VALUES ((" << 
			"SELECT id FROM users WHERE username = '" << username << "'), '" <<
			GROUP_DEFAULT_NAME << "');";
		
		if (stmt->executeUpdate(query.str()) != 1)
			return false;
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.str().c_str());	
		
	} catch(...) {
		cout << "Exception in add_default_group!!!" << endl;
		return false;
	}
	return true;
}

string Server::get_list_of_friends(string username) {
	stringstream query;
	string friends;

	try {			
		query.flush();
		query << "SELECT " << GROUPS_T_FRIENDS_LIST << " FROM groups WHERE " << GROUPS_T_FRIENDS_LIST << " IS NOT NULL AND " << GROUPS_T_ID_USER_FK << " = " << 
			"(SELECT id FROM users WHERE username = '" << username << "');";
		
		sql::ResultSet * res = stmt->executeQuery(query.str());		
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.str().c_str());	
		
		if (res->next()) {
			sql::SQLString friends_list = res->getString(GROUPS_T_FRIENDS_LIST);
			cout << "[SERVER] list is: " << friends_list << endl << flush;
			friends = string(friends_list);
			
		}
		else
			friends = string("");
		
		delete res;
		
	} catch(...) {
		cout << "Exception in get_list_of_friends!!!" << endl;
		return "";
	}
	
	return friends;
}

bool Server::register_client(int sockfd, string username, string pass, string email) {

	dprintf("[SERVER]received register command\n");
	int rc = true;
	
	//1)query database and check for username
	string query = string("SELECT username, email "
				"FROM users "
				"WHERE username = '").append(username).
				append("' OR email = '").append(email).
				append("';");
	cout << query << endl;
	
	try {
		sql::ResultSet * res = stmt->executeQuery(query);
		//2)if username or email exists, send USEDUSER | USEDMAIL | ERR on sockfd
		if (res->next()) {
			dprintf("[SERVER]invalid client info %s %s\n", username.c_str(), email.c_str());
			if (username.compare(res->getString("username")) == 0) {
				assert(send(sockfd, USEDUSER_ERR, strlen(USEDUSER_ERR) + 1, 0) >= 0);
				rc = false;
			}
			else if (email.compare(res->getString("email")) == 0) {
				assert(send(sockfd, USEDEMAIL_ERR, strlen(USEDEMAIL_ERR) + 1, 0) >= 0);
				rc = false;
			}
		}
		else {
			//3)if username is valid, add to db, create default group for friends and send OK on sockfd
			dprintf("[SERVER]adding %s\n", username.c_str());
			query = string("INSERT INTO users(username, ").append(PASS_FIELD).append(", email)")
					.append(" VALUES ('").append(username).append("', '").
					append(pass).append("', '").append(email + "');");
			cout << query << endl << flush;
			stmt->executeUpdate(query);
			dprintf("[SERVER]added %s\n", username.c_str());
			
			if (!add_default_group(username)) {
				stmt->executeUpdate("DELETE FROM users WHERE username = '" + username + "';");
				assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);	
			}
			
			assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);
		}

		delete res;
	}catch (sql::SQLException &e) {
		rc = false;
	}

	return rc;
}

/* Login user <username>; if login succeeds, store info of him and send him OK, otherwise send FAIL. */
bool Server::authentication(int sockfd, std::string username, std::string pass, std::string ip, int port) {
	int rc = true;
	ClientInfo * ci;
	
	dprintf("[SERVER]received login request for '%s' with pass '%s'\n", username.c_str(), pass.c_str());
	string query = string("SELECT ").append(PASS_FIELD).
					append(" FROM users "
					"WHERE username = '").append(username).append("';");

	//query db and compare pass
	//send FAIL/(more info<groups and users, offline messages> + END) on sockfd
	try {
		sql::ResultSet * res = stmt->executeQuery(query);
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.c_str());
		if (res->next() && string(res->getString(PASS_FIELD)).compare(pass) == 0) {
				//TODO query db for client' friends and offline msg and send to client
				char end[] = "END";
				assert(send(sockfd, end, strlen(end) + 1, 0) >= 0);
				
				//update username (//TODO update status too?) of client with key sockfd
				ci = get_client_info(sockfd);
				ci->set_username(username);
		}
		else {
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
			rc = false;
		}
		
		
		if (res)
			delete res;
			
	}catch (sql::SQLException &e) {
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
		rc = false;
	}

	return rc;
}

// Add/Remove/Search a user for a client
bool Server::add_user(int sockfd, std::string username) {
	//TODO
}
bool Server::remove_user(int sockfd, std::string username) {
	//TODO
	
}
bool Server::search_user(int sockfd, std::string name, std::string surname,
			std::string phone, std::string email, std::string hobbies) 
{
	//TODO
}

