/*
* author radu
* untested functions
*
*/




bool Server::add_user(int sockfd, std::string username) {
	int rc = true;
	int myid,def_group_id;
	bool exits=false;
	sql::ResultSet * res;
	dprintf("[SERVER]received adduser request for '%s' from'%s'\n", username.c_str(), myusername.c_str());

	//string myusername = this->sockfd_to_clients.find(sockfd); => iterator TODO
	string query = string("SELECT").append(GROUPS_T_ID).append( " from USERS where username = \"").append(myusername).append("\";");


	try {
		res = stmt->executeQuery(query);
		if(res==null) { dprintf("[SERVER] myuser not good\n");
				return false;
				}
		res->next();
		myid=res->getInt(0);
		dprintf("[SERVER]myid query executed myid=%i\n",id);
		delete res;
		query=string("SELECT ").append(GROUPS_T_FRIENDS_LIST).append(" FROM groups where ").append(GROUPS_T_ID_USER_FK).append("=").append(myid).append(";");
		res = stmt->executeQuery(query);
		dprintf("[SERVER]groups list query executed \n",id);
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
				strcpy(cstr,str.c_str());
				p=strtok(cstr," ");

				 while (p!=NULL)
  				{
   			 		if(strcmp(p,username)==0)
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
		if(!exists)
		{
			/*add_user to default group
			*/
			query=string("UPDATE groups SET ").append( GROUPS_T_FRIENDS_LIST).append("=concat(").append( GROUPS_T_FRIENDS_LIST).append(",").append(username).append(") where ").append(GROUPS_T_ID_USER_FK).append("=").append(myid).append(" AND ").append(GROUPS_T_NAME).append(" = ").append(GROUP_DEFAULT_NAME).append(";");
			int modified=stmt->executeUpdate(query);

			if(modified)
				dprintf("[SERVER]added to %i group \n",modified);
			else {
				dprintf("[SERVER] myuser not good\n");
				return false;
				}


		}
	}
	catch(sql::SQLException &e) {
		assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
		rc = false;
	}

	return rc;

}
