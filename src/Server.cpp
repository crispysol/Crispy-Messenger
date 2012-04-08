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

#include "ServerFunctions.h"
#include "Server.h"

using namespace std;

ClientInfo::ClientInfo(std::string ip, int port) : ip(ip), port(port)
{
}


ClientInfo::~ClientInfo() {}

Server::Server() {
	sql::mysql::MySQL_Driver *driver;
	
	try{
		driver = sql::mysql::get_mysql_driver_instance();
		con = driver->connect(DATABASE_HOST, DATABASE_USER, DATABASE_PASS);
		con->setSchema(DATABASE_NAME);

		delete driver;
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

bool Server::register_client(int sockfd, std::string username, std::string pass, std::string email) {
	int rc = true;
	sql::Statement	*stmt;
	//1)query database and check for username
	string query = string("SELECT username, email "
						"FROM users "
						"WHERE username == ").append(username);
	stmt = con->createStatement();
	
	try {
		sql::ResultSet * res = stmt->executeQuery(query);	
		//2)if username or email exists, send USEDUSER | USEDMAIL | ERR on sockfd
		if (res != NULL) {
			if (res->getString("username") != NULL) {
				assert(send(sockfd, USEDUSER_ERR, strlen(USEDUSER_ERR), 0) >= 0);
				rc = false;
			}
			else if (res->getString("email") != NULL) {
				assert(send(sockfd, USEDEMAIL_ERR, strlen(USEDEMAIL_ERR), 0) >= 0);
				rc = false;
			}
			else {
				//3)if username is valid, add to db and send OK on sockfd
				query = string("INSERT INTO ").append("users(username, pass, email)").append(" VALUES (").
					append(username).append(", ").append(pass).append(", ").append(email + ")");
				stmt->execute(query);
				assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG), 0) >= 0);
			}

			delete res;
		}
		else {
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG), 0) >= 0);
			rc = false;
		}
	}catch (sql::SQLException &e) {
		rc = false;
	}
		
	delete stmt;

	return rc;
}

bool Server::authentication(int sockfd, std::string username, std::string pass, std::string ip, int port) {
	int rc = true;
	sql::Statement	*stmt;
	string query = string("SELECT pass "
							"FROM users "
							"WHERE username == ").append(username);
	stmt = con->createStatement();
	//query db and compare pass
	//send FAIL/(more info<groups and users, offline messages> + END) on sockfd
	try {
		sql::ResultSet * res = stmt->executeQuery(query);
		if (res != NULL && string(res->getString("pass")).compare(pass) == 0) {
				//save client info (ip and port) to session
				ClientInfo ci = ClientInfo(ip, port);
				clients.insert(pair<int, ClientInfo>(sockfd, ci));
				//query db for client' friends and offline msg and send to client
				assert(send(sockfd, "END", 3, 0) >= 0);
		}
		else {
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG), 0) >= 0);
			rc = false;
		}
		if (res)
			delete res;
	}catch (sql::SQLException &e) {
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG), 0) >= 0);
		rc = false;
	}
	
	delete stmt;

	return rc;
}

