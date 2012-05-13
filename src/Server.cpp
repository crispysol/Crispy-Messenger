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
#include <algorithm>
#include <vector>

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

map <int, ClientInfo*> & Server::get_sockfd_to_clients() {
	return sockfd_to_clients;
}

map <std::string, int> & Server::get_clients_to_sockfd() {
	return clients_to_sockfd;
}

void Server::insert_in_sockfd_to_clients(int key, ClientInfo * ci) {
	sockfd_to_clients.insert(pair<int, ClientInfo*> (key, ci));
}

ClientInfo * Server::get_clientInfo_by_sockfd(int sockfd) {
	map<int, ClientInfo*>::iterator it_fdcl;
	
	it_fdcl = sockfd_to_clients.find(sockfd);
	assert(it_fdcl != sockfd_to_clients.end());
	
	return it_fdcl->second;
}

/**
 * Add default group for user <username>. 
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

/**
 * Get list of friends within each group of friends of user <username>. 
 */
map<string, string> Server::get_list_of_friends(string username) {
	stringstream query;
	map<string, string> friends;

	try {			
		query.flush();
		query << "SELECT " << GROUPS_T_NAME << ", " << GROUPS_T_FRIENDS_LIST << " FROM groups WHERE " <<
			GROUPS_T_ID_USER_FK << " = " << "(SELECT id FROM users WHERE username = '" << username << "');";
		
		sql::ResultSet * res = stmt->executeQuery(query.str());		
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.str().c_str());	

		// For each group of friends in the db
		while (res->next()) {
			sql::SQLString group = res->getString(GROUPS_T_NAME);
			sql::SQLString friends_list = res->getString(GROUPS_T_FRIENDS_LIST);
			cout << "[SERVER] list ( " << group << " ) is: " << friends_list << endl << flush;
			friends.insert(pair<string, string> (string(group), string(friends_list)));
		}
		
		delete res;
		
	} catch(...) {
		cout << "Exception in get_list_of_friends!!!" << endl;
	}
	
	return friends;
}

/** 
 * Register client to db if data (username and email) is valid, create default group and send back operations' result. 
 * If any of the operations fails, roll back and send coresponding message to client.
 */
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
		//2)if username or email exists, send USEDUSER | USEDMAIL | ERR
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

static void tokenize(const string& str, vector<string>& tokens, const string& delimiters = " ") {
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos) {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

/**
 * Send initial info at login to user that just logged
 */
bool Server::send_initial_info(int sockfd, string username) {
	stringstream ss;

	// Retrieve friends
	map<string, string> friends = this->get_list_of_friends(username);
	map<string, string>::iterator it = friends.begin(), it_end = friends.end();

	// Error (should not receive this case)
	if (it == it_end) {
		cerr << "[BUG] Error at retrieving groups from database for user: " << username << endl;
		return false;
	}

	// Add groups to message
	ss << "{ \"groups\": [";
	bool first = false, first_tok;
	for (; it != it_end; it++) {
		// Add group name
		if (!first) {
			first = true;
		} else {
			ss << ", ";
		}
		ss << endl << "{\"name\": \"" << it->first << "\", \"users\": [";

		// Add users to message
		vector<string> tokens;
		tokenize(it->second, tokens, ", ");
		vector<string>::iterator tok = tokens.begin(), tok_end = tokens.end();
		first_tok = false;
		for (; tok != tok_end; tok++) {
			// Add user name
			if (!first_tok) {
				first_tok = true;
			} else {
				ss << ", ";
			}
			ss << "{\"name\": \"" << *tok << "\", ";
			// Add state and status
			std::map <std::string, int>::iterator cli_sock = clients_to_sockfd.find(username);
			string state = "offline", status = NO_STATUS;
			if (cli_sock != clients_to_sockfd.end()) {
				std::map <int, ClientInfo*>::iterator cli = sockfd_to_clients.find(cli_sock->second);
				if (cli != sockfd_to_clients.end()) {
					ClientInfo * ci = cli->second;
					state = ci->get_state_as_string();
					status = ci->get_status();
				}
			}
			ss << "\"state\": \"" << state << "\", \"status\": \"" << status << "\"";
			ss << "}";
		}
		ss << "]}";
	}
	ss << endl << "]," << endl;

	// Add offline messages TODO
	ss << "\"offline_messages\": \"TODO\"" << endl << "}" << endl;

	// Send message
	assert(send(sockfd, ss.str().c_str(), ss.str().length() + 1, 0) >= 0);

	return true;
}

/**
 * Login user <username>; if login succeeds, store info of him and send him OK, otherwise send FAIL. 
 */
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
				ci = get_clientInfo_by_sockfd(sockfd);
				ci->set_username(username);
				ci->set_state(AVAILABLE);
				ci->set_status("");

				// Send initial info TODO
				send_initial_info(sockfd, username);
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


/*
* Verify if a username is in the database
*/
static bool username_exists(std::string username)
{
	bool exists = false;
	dprintf("[SERVER] Verifying if username '%s' exists\n", username.c_str());
	string query = string("SELECT id from users where username = \"").append(username).append("\";");

	try {
		sql::ResultSet *  res = stmt->executeQuery(query);
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.c_str());
		if(res->rowsCount()==1)
				exists	= true;
		delete res;
	 }

	catch(sql::SQLException &e) {
		
		
		dprintf("sql exception\n");
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		
	}

	
	return exists;
}

// Add/Remove/Search a user for a client
bool Server::add_user(int sockfd, std::string username) {
	int rc = true;
	int myid,def_group_id;
	bool exists=false;
	std::stringstream out_id;
	sql::ResultSet * res;
	string myusername;
	ClientInfo * my_client;
	dprintf("[SERVER]received adduser request\n");
	my_client=get_clientInfo_by_sockfd(sockfd);
	myusername=my_client->get_username();
	if(myusername == "") {
					assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
					return false;
				}
	
	dprintf("[SERVER]received adduser request for '%s' from'%s'\n", username.c_str(), myusername.c_str());
	/*if the person i'm trying to add doesn't exist, error
	*/	
	if(!username_exists(username)) {
					dprintf("[SERVER] username %s doesn't exist\n", username.c_str());
					assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
					return false;
					

					}
	string query = string("SELECT ").append(GROUPS_T_ID).append( " from users where username = \"").append(myusername).append("\";");


	try {
		res = stmt->executeQuery(query);
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.c_str());
		if(res->rowsCount()!=1) { dprintf("[SERVER] myuser not good/ problem\n");
				assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);				
				return false;
				}
		res->next();
		myid=res->getInt(1);
		dprintf("[SERVER]myid query executed myid=%i\n",myid);
		out_id << myid;
		delete res;
		query=string("SELECT ").append(GROUPS_T_FRIENDS_LIST).append(" FROM groups where ").append(GROUPS_T_ID_USER_FK).append("=").append(out_id.str()).append(";");
		res = stmt->executeQuery(query);
		dprintf("[SERVER]groups list query executed \n");
		if(res->rowsCount()>0)
		{
			res->beforeFirst();
			/*Caut in fiecare rand , in lista de prieteni numele prietenului pe care il adaug
			*/
			while (res->next() &&!exists)
			{
				string friends=res->getString(GROUPS_T_FRIENDS_LIST);
				char *p, *cstr;

				cstr=new char[friends.size()+1];
				strcpy(cstr,friends.c_str());
				p=strtok(cstr," ");

				 while (p!=NULL)
  				{
   			 		if(strcmp(p,username.c_str())==0)
					{//username already is in my list
					 	exists=true;
					 	break;
					}
   			 		p=strtok(NULL," ");
  				}

  				delete[] cstr;
			}
		}
		delete res;
	/*
	*if user doesn't exist in the clients friend list, i will add him
	*
	*/
		if (!exists)
		{
			/*add_user to default group
			*/
			query=string("UPDATE groups SET ").append( GROUPS_T_FRIENDS_LIST).append("=concat(").append( GROUPS_T_FRIENDS_LIST).append(",\"").append(username).append(",\") where ").append(GROUPS_T_ID_USER_FK).append("=").append(out_id.str()).append(" AND ").append(GROUPS_T_NAME).append(" = \"").append(GROUP_DEFAULT_NAME).append("\";");
			dprintf("%s\n",query.c_str());\
			int modified=stmt->executeUpdate(query);
			dprintf("[%s executed]%s\n", SQL_DEBUG, query.c_str());
			/*if (modified) 
				dprintf("[SERVER]added to %i group \n",modified);
				
			else {
				dprintf("[SERVER] myuser not good\n");
				return false;
				}
			
*/
		}
		else assert(send(sockfd, USER_ALREADY_IN_LIST, strlen(USER_ALREADY_IN_LIST) + 1, 0) >= 0);
	}
	catch(sql::SQLException &e) {
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
		
		dprintf("sql exception\n");
		cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		rc = false;
	}

	if(rc == true) assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);

	else assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);

	return rc;

}

bool Server::remove_user(int sockfd, std::string username) {

	int rc = true;
	int def_group_id;
	bool exists=false;
	std::stringstream friends_list_q;
	sql::ResultSet * res;
	string myusername;
	ClientInfo * my_client;
	
	my_client=get_clientInfo_by_sockfd(sockfd);
	myusername=my_client->get_username();
	if(myusername == "") {
					assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
					return false;
				}
	
	dprintf("[SERVER]received remove request for '%s' from'%s'\n", username.c_str(), myusername.c_str());
	/*if the person i'm trying to remove doesn't exist, error
	*/
	//TODO	when a user is removed from db, user should be removed also from all lists of friends of other users
	if(!username_exists(username)) {
					dprintf("[SERVER] user %s isn't you friend\n", username.c_str());
					assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
					return false;
					

					}	
					
	// Retrieve list of friends containing <username>
	string friends;
	friends_list_q << "SELECT " << GROUPS_T_ID << "," << GROUPS_T_FRIENDS_LIST <<  
		" FROM groups WHERE id_user = (SELECT id FROM users WHERE username = '" << myusername << 
		"') AND (SELECT friends_list REGEXP '(," << username << ")$|^(" << username << ",)|^" << username << 
		"$|(.+," << username << ",.+)') = 1;";
		
	try {
		sql::ResultSet *  res = stmt->executeQuery(friends_list_q.str());
		dprintf("[%s executed]%s\n", SQL_DEBUG, friends_list_q.str().c_str());
		if(res->next()) {
			friends = string(res->getString(GROUPS_T_FRIENDS_LIST));
			def_group_id = atoi(string(res->getString(GROUPS_T_ID)).c_str());
			
			// erase <username> in friends list (cases:list is "[other_user,...]<username>[,...]")
			int pos, maxlen, length;
			for (pos = friends.find(username), maxlen = friends.length(), length = username.length(); 
							pos != string::npos; pos = friends.find(username, pos + 1))
				if (pos + length < maxlen)
					if (friends[pos + length] == ',')
						if (pos > 0)
							if (friends[pos - 1] == ',') {
								friends.erase(pos, length + 1);
								break;
							}
							else continue;
						else {
							friends.erase(pos, length + 1);
							break;
						}
					else continue;
				else
					if (pos > 0)
						if (friends[pos - 1] == ',') {
							friends.erase(pos - 1, length + 1);
							break;
						}
						else continue;
					else {
						friends.clear();
						break;
					}
	
			// Update db
			friends_list_q.flush();
			friends_list_q << "UPDATE groups SET friends_list = '" << friends << "' WHERE " << 
				GROUPS_T_ID << " = " << def_group_id << ";";
			dprintf("[%s execute]%s\n", SQL_DEBUG, friends_list_q.str().c_str());
			if (stmt->executeUpdate(friends_list_q.str()) == 0)
				rc = false;
			else 	rc = true;
		}
		else
			rc = false;
			
		
		delete res;
	}catch(...) {
		dprintf("Exception in remove_user!!!\n");
		rc = false;
	}
	
	if (!rc)
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
	else
		assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);
		
	return rc;
}

bool Server::search_user(int sockfd, std::string name, std::string surname,
			std::string phone, std::string email, std::string hobbies) 
{
	//TODO
}

