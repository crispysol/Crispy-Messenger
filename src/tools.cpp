/*
* author radu
* untested functions
*
*/
bool Server::add_user(int sockfd, std::string username) {
	int rc = true;
	int myid,def_group_id;
	bool exists=false;
	std::stringstream out_id;

	sql::ResultSet * res;
	string myusername; //TODO iau din json??
	//string myusername = this->sockfd_to_clients.find(sockfd); //=> iterator TODO
	dprintf("[SERVER]received adduser request for '%s' from'%s'\n", username.c_str(), myusername.c_str());
	
	
	string query = string("SELECT ").append(GROUPS_T_ID).append( " from users where username = \"").append(myusername).append("\";");


	try {
		res = stmt->executeQuery(query);
		dprintf("[%s executed]%s\n", SQL_DEBUG, query.c_str());
		if(res==NULL) { dprintf("[SERVER] myuser not good\n");
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
			query=string("UPDATE groups SET ").append( GROUPS_T_FRIENDS_LIST).append("=concat(").append( GROUPS_T_FRIENDS_LIST).append(",\"").append(username).append("\") where ").append(GROUPS_T_ID_USER_FK).append("=").append(out_id.str()).append(" AND ").append(GROUPS_T_NAME).append(" = \"").append(GROUP_DEFAULT_NAME).append("\";");
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
		else assert(send(sockfd, ERR_MSG, strlen(USER_ALREADY_IN_LIST) + 1, 0) >= 0);
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

	return rc;

}

//in server main, to recognize add_user command


/*
where CMD_ADD_USER =add_user in serverfunctions.h

if(line.find(CMD_ADD_USER) == 0){
	dprintf("sending add_user command\n");
	int user_pos = line.find(" ") + 1;
	server->add_user(sockfd,line.substr(user_pos));
	return;
	}
	dprintf("%s\n",line.c_str());
*/


bool Client:add_user(std::string username)
{
	int rc;
	char buffer[BUFFER_LENGTH];

	//send username,  to server_socket
	char msg[BUFFER_LENGTH];
	sprintf(msg, "%s %s", CMD_REGISTER, username.c_str());
	assert(send(server_socket, msg, strlen(msg) + 1, 0) >= 0);

	//receive response from server_socket
	rc = recv(server_socket, buffer, sizeof(buffer), 0);
	assert(rc >= 0);
	dprintf("[CLIENT]received from server: %s\n", buffer);
	if (rc == 0 || strcmp(buffer, ERR_MSG) == 0 || strcm(buffer, USER_ALREADY_IN_LIST) == 0)
		return false;

	return true;
}
