/*
 * Functions.h
 *
 *  Created on: May 5, 2011
 *      Author: cmihail
 */

/**
 * Receive a new connection and add it to read_fds
 */
void new_connection(int sockfd, int & fdmax, fd_set * read_fds);

/**
 * End a connection and remove it from read_fds
 */
void end_connection(int sockfd, fd_set * read_fds);

/**
 * Init server
 */
void init_server(int & server_port, int & sockfd, int & fdmax, fd_set * read_fds);

/**
 * Connect to a server (only for client)
 */
void connect_to_server(char * server_ip, int server_port,  int & socket_server, int & fdmax, fd_set * read_fds);
