/*
 * ClientGTK.cpp
 *
 *  Created on: Mar 13, 2012
 *      Author: mihail
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <regex.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms-compat.h>

#include "../ServerFunctions.h"
#include "../Client.h"
#include "ClientGTK.h"
#include "Emoticons.h"

// Defines
#define DEFAULT_IP		"127.0.0.1"
#define DEFAULT_PORT	5000

using namespace std;

// Default values for port and ip
static string ip = DEFAULT_IP;
static int port = DEFAULT_PORT;

// Local server info
static int sockfd;
static fd_set read_fds;
static fd_set tmp_fds;
static int fdmax;
// Server socket
static int socket_server, client_port;

// Client info
Client * current_client;

// Map used for correspondence between client - chat window
map <string, GtkWidget *> map_chat_windows;
map <string, GtkWidget *> map_chat_text;

// Gtk widgets
GtkWidget * main_window_top_level;
GtkWidget * main_window_vbox;

// Emoticons map
map <string, string> emoticons;

/**
 * Check login values and executte create main interface if everything is ok
 */
void signal_check_login(struct _general_info * g_info) {
	// Save window top level
	GtkWidget * window_top_level = g_info->window_top_level;

	// Save username and password
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->username)));
	gchar * password = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->password1)));

	// Login
	if (!current_client->authentication(username, password)) {
		clientgtk_create_message_dialog("Invalid login username/password", "Login information",
				GTK_MESSAGE_WARNING);

		// Free space
		free(username);
		free(password);
		return;
	}

	// Memorize my username
	current_client->set_username(username);
	current_client->set_state(AVAILABLE);
	current_client->set_status("");

	// Delete login interface
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(username);
	free(password);
	free(g_info);

	// Create main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Check register values and register client
 */
void signal_check_register(struct _general_info * g_info) {
	// Save username and password
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->username)));
	gchar * password1 = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->password1)));
	gchar * password2 = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->password2)));
	gchar * email = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->email)));

	// Variable used for errors
	bool error = false;

	// Check if fields have the necessary length
	if (strlen(username) < MIN_REGISTER_CHARS || strlen(password1) < MIN_REGISTER_CHARS
			|| strlen(password2) < MIN_REGISTER_CHARS) {
		stringstream ss(stringstream::out);
		ss << "The username/password fields must have at least " << MIN_REGISTER_CHARS <<
				" characters!";
		clientgtk_create_message_dialog(ss.str().c_str(), "Error register!", GTK_MESSAGE_ERROR);
		error = true;
	}

	// Check if passwords are the same
	if (!error && memcmp(password1, password2, strlen(password1))) {
		clientgtk_create_message_dialog("The password fields must be the same!", "Error register!",
				GTK_MESSAGE_ERROR);
		error = true;
	}

	// Check if email is correct
	regex_t preg;
	regcomp(&preg, ".*[@].*[.].*", REG_EXTENDED);
	if (!error && regexec(&preg, email, 0, NULL, 0)) {
		clientgtk_create_message_dialog("Email field is incorrect", "Error register!",
				GTK_MESSAGE_ERROR);
		error = true;
	}

	// Check if there are any errors
	if (!error) {
		// Register client
		if (current_client->register_client(username, password1, email)) {
			clientgtk_create_message_dialog("Account created", "Register information",
					GTK_MESSAGE_INFO);

			// Delete register interface
			gtk_widget_destroy(g_info->vbox_align);

			// Change interface back to login
			clientgtk_create_login_window(g_info->window_top_level);

			// Free space used by register interface
			free(g_info);
		} else {
			clientgtk_create_message_dialog("Account already exists", "Register information",
					GTK_MESSAGE_WARNING);
		}
	}

	// Free space
	free(username);
	free(password1);
	free(password2);
	free(email);
}

/**
 * Check recovery values and TODO (do something)
 */
void signal_check_recovery(struct _general_info * g_info) {
	// Save email
	gchar * email = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->email)));

	// Variable used for errors
	bool error = false;

	// Check if email is correct
	regex_t preg;
	regcomp(&preg, ".*[@].*[.].*", REG_EXTENDED);
	if (!error && regexec(&preg, email, 0, NULL, 0)) {
		clientgtk_create_message_dialog("Email field is incorrect", "Error recovery!",
				GTK_MESSAGE_ERROR);
		error = true;
	}

	// Check if there are any errors
	if (!error) {
		// Recover account information TODO must retrieve answer too
		// TODO function does not exist, must be created in Client.h !!!

		// Show information about recovery // TODO check answer from TODO to see if everything is ok
		clientgtk_create_message_dialog("Account information recovered", "Recovery information",
				GTK_MESSAGE_INFO);

		// Delete recovery interface
		gtk_widget_destroy(g_info->vbox_align);

		// Change interface back to login
		clientgtk_create_login_window(g_info->window_top_level);

		// Free space used by recovery interface
		free(g_info);
	}

	// Free space
	free(email);
}

/**
 * Select file for sending
 */
void signal_send_file(GtkWidget * widget, gpointer g_client) {
	char * client = (char *) g_client;

	// File dialog chooser
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Choose file ...", NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);

	string filename;
	if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_OK) {
		filename = string(gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser)));
	}
	dprintf("Send file %s to %s\n", filename.c_str(), client);

	// Connect with user
	current_client->connect_with_user_req(client);
	sleep(1);
	//while (current_client->get_socket_of_connected_user(client) < 0); TODO

	// Send file
	if (!current_client->send_file(filename, client)) {
		clientgtk_create_message_dialog("Error at send file", "Error", GTK_MESSAGE_ERROR);
	}

	// Destroy file dialog chooser
	gtk_widget_destroy(chooser);
}

/**
 * Send text to friend and save it in conversation text view
 */
gboolean signal_send_text(GtkWidget * entry_chat, GdkEventKey * event, gpointer info) {
	if (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter) {
		struct _general_info * g_info = (struct _general_info *) info;
		GtkWidget * conversation_chat = (GtkWidget *) g_info->window;

		// Get text from input
		GtkTextBuffer * buffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_chat));
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer1, &start, &end);
		gchar * new_text = gtk_text_buffer_get_text(buffer1, &start, &end, FALSE);
		string username = current_client->get_username();
		string text = username + ": " + string(new_text) + "\n";

		// Split text based on emoticons
		vector <string> strs = emoticons_split_text(text, emoticons);

		// Print message and emoticons
		emoticons_print_text(conversation_chat, strs, emoticons);

		// Send message to friend
		current_client->send_message(g_info->friend_username, new_text);

		// Free space
		free(new_text);

		// Delete all text from input
		gtk_text_buffer_set_text(buffer1, "", -1);

		return TRUE;
	}
	return FALSE;
}

/**
 * Logout
 */
void signal_logout(GtkWidget * widget, gpointer info) {
	struct _general_info * g_info = (struct _general_info *) info;

	// Destroy old interface
	gtk_widget_destroy(g_info->vbox_align);

	// Destroy all chat windows
	map <string, GtkWidget *>::iterator it = map_chat_windows.begin(),
			it_end = map_chat_windows.end();
	for (; it != it_end; it++) {
		gtk_widget_destroy(it->second);
	}

	// Create login interface
	clientgtk_create_login_window(g_info->window_top_level);

	// TODO logout from server

	// Free space
	free(g_info);
}

/**
 * Receive a message
 */
void receive_msg(string friend_username, string message) {
	gdk_threads_enter();
	map <string, GtkWidget *>::iterator it = map_chat_text.find(friend_username);
	if (it == map_chat_text.end()) {
		clientgtk_create_chat_window(NULL, (gpointer) friend_username.c_str());
		it = map_chat_text.find(friend_username);
	}

	// Set text to conversation box
	GtkWidget * conversation_chat = it->second;
	string username = current_client->get_username();
	string text = username + ": " + message + "\n";

	// Split text based on emoticons
	vector <string> strs = emoticons_split_text(text, emoticons);

	// Print message and emoticons
	emoticons_print_text(conversation_chat, strs, emoticons);
	gdk_threads_leave();
}

/**
 * Process message from server.
 */
static void process_server_msg(string buffer, int & fdmax, fd_set * read_fds) {
	if (buffer.find(SUCCESS_MSG) == 0)
		return;

	if (buffer.find(CMD_SEND_MSG) == 0) {
		int name_dst = buffer.find(" ") + 1;
		int msg_pos = buffer.find(" ", name_dst) + 1;

		receive_msg(buffer.substr(name_dst, msg_pos - 1 - name_dst), buffer.substr(msg_pos));
		return;
	}

	if (buffer.find(FRIEND_IS_ONLINE) == 0) {
		gdk_threads_enter();

		// Receive friends
		Json::Value root;
		if (!current_client->receive_friend_list(root)) {
			return;
		}

		// Remake main interface
		gtk_widget_destroy(main_window_vbox);
		clientgtk_create_main_window(main_window_top_level);
		gdk_threads_leave();
		return;
	}

	if(buffer.find(CMD_CONN_CLIENT_TO_CLIENT_RES) == 0) {
		// connect with user
		int sockfd = current_client->connect_with_user_res(buffer, fdmax, read_fds);
		dprintf("[DONE, sockfd %i]%s\n", sockfd, buffer.c_str());
		if (sockfd == -1)
			return;

		//tell the other end (client to connect to) my username
		char msg[BUFFER_LENGTH];
		sprintf(msg, "%s %s", CMD_CONN_REQ_FROM, current_client->get_username().c_str());
		assert(send(sockfd, msg, strlen(msg) + 1, 0) >= 0);

		return;
	}

	if (buffer.find(ONLINE_FRIEND_INFO) == 0) {
		cout << "[CLIENT] ONLINE FRIEND INFO: " << buffer;
		Json::Value root;

		if (!current_client->receive_friend_list(root)) {
			return;
		}
	}
}

/**
 * Process message from other client.
 */
static void client_command(string buffer, int sockfd) {
	if (buffer.find(CMD_CONN_REQ_FROM) == 0) {
		//get username of client which initiated connection
		char username[BUFFER_LENGTH];
		int user_pos = buffer.find(' ') + 1;

		if (user_pos == 0) {
			dprintf("[CLIENT]%s wrong format (correct format is '%s username')\n", CMD_CONN_REQ_FROM,
				CMD_CONN_REQ_FROM);
			assert(send(sockfd, ERR_MSG, strlen(ERR_MSG) + 1, 0) >= 0);
			return;
		}

		// store socket of the other end
		memset(username, 0, strlen(username));
		buffer.copy(username, buffer.length() - user_pos, user_pos);
		dprintf("[CLIENT]connected with %s\n", username);
		current_client->insert_in_connected_users(username, sockfd);

		sprintf(username, "%s %s", CONNECTED_MSG, current_client->get_username().c_str());

		// Tell the other end connection is established.
		assert(send(sockfd, username, strlen(username) + 1, 0) >= 0);

		return;
	}

	if (buffer.find(FILE_TRANSFER) == 0) {
		// send success receiving file header
		assert(send(sockfd, SUCCESS_MSG, strlen(SUCCESS_MSG) + 1, 0) >= 0);

		// get size from header
		vector<string> tokens;
		tokenize(buffer, tokens, " ");
		vector<string>::iterator tok = tokens.begin();
		tok+=2;
		int size = atoi((*tok).c_str());
		dprintf("size is %u\n", size);

		// receive file
		char *file = (char*)malloc(size);
		assert(file != NULL);
		int n = recv(sockfd, file, size, MSG_WAITALL);
		dprintf("read %i\n", n);

		// write file
		vector<string> file_tokens;
		tokenize(*(tokens.begin()+1), file_tokens, "/");
		string filename = file_tokens.at(file_tokens.size() - 1);
		ofstream out(filename.c_str());
		out << file;
		out.close();

		dprintf("RECEIVED FILE: %s (%s)\n", filename.c_str(), (*(tokens.begin()+1)).c_str());

		free(file);
		return;
	}

	if (buffer.find(CONNECTED_MSG) == 0) {
		int user_pos 	= buffer.find(" ") + 1;
		string username = buffer.substr(user_pos);
		dprintf("[CLIENT]just start sending file to %s\n", username.c_str());
		current_client->insert_in_connected_users(username, sockfd);
		return;
	}

	if (buffer.find(ERR_MSG) == 0) {
		dprintf("[CLIENT]cannot start sending file :( error encountered\n");
		return;
	}

}

/**
 * Local server run
 */
void * start_thread(void * ptr_thread) {
	char buffer[BUFFER_LENGTH];
	int n, newsockfd;
	string ip;

	// Main loop
	FD_ZERO(&tmp_fds);
	for (;;) {
		tmp_fds = read_fds;
		assert(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) != -1);

		// Check every socket for new data
		for (int i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &tmp_fds)) {
				// New connection
				if (i == sockfd) {
					dprintf("received new connection on %i\n", i);
					new_connection(sockfd, fdmax, &read_fds, ip, newsockfd);
					continue;
				}

				// Execute command from STDIN (none for GTK)
				if (i == STDIN_FILENO) {
					continue;
				}

				// Receive data from server
				if (i == socket_server) {
					n = recv(i, buffer, sizeof(buffer), 0);
					if (n <= 0) {
						dprintf("server hung up\n");
						end_connection(i, &read_fds);
						goto exit_label;
						break;
					}

					// Received data from server
					process_server_msg(string(buffer), fdmax, &read_fds);
					continue;
				}

				// Received data from another client
				if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
					end_connection(i, &read_fds);
					current_client->remove_from_connected_users(i);
				} else {
					dprintf("received from client: %s\n", buffer);
					client_command(string(buffer), i);
				}
			}
		}
	}
exit_label:
	close(sockfd);
	return 0;
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
	// Change server's ip / port
	if (argc == 3) {
		ip = argv[1];
		port = atoi(argv[2]);
	}

	// Init localhost server (communication with other clients)
	init_server(client_port, sockfd, fdmax, &read_fds);

	// Connect to main server
	char aux_ip[strlen(ip.c_str()) + 1];
	strcpy(aux_ip, ip.c_str());
	connect_to_server(aux_ip, port, socket_server, fdmax, &read_fds);

	// Send port to server
	char buffer[BUFFER_LENGTH];
	sprintf(buffer, "%s %i", INFO_CLIENT_PORT, client_port);
	dprintf("sending port %i to server\n", client_port);
	assert(send(socket_server, buffer, strlen(buffer) + 1, 0) >= 0);

	// Create current client
	current_client = new Client(socket_server);

	// Create emoticons
	emoticons = emoticons_create_map(string(EMOTICONS_FOLDER) + string(EMOTICONS_FILE));

	// Init GTK
	g_thread_init(NULL);
	gdk_threads_init();
	gdk_threads_enter();
	gtk_init(&argc, &argv);

	// Create main window
	GtkWidget * window_top_level = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_top_level), WINDOW_TITLE);
	gtk_window_set_default_size(GTK_WINDOW(window_top_level), WINDOW_WIDTH, WINDOW_HEIGHT);
	//gtk_window_set_position(GTK_WINDOW(window_top_level), GTK_WIN_POS_CENTER); TODO uncomment

	// Set main window bg color
	GdkRGBA window_color;
	gdk_rgba_parse(&window_color, WINDOW_BG_COLOR);
	gtk_widget_override_background_color(window_top_level,
			(GtkStateFlags) GTK_STATE_FLAG_NORMAL, &window_color);

	// Show top level window
	gtk_widget_show(window_top_level);

	// Signal to kill main gtk loop
	g_signal_connect(window_top_level, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Create login interface
	clientgtk_create_login_window(window_top_level);

	// Add thread
	pthread_t tread_id;
	pthread_create(&tread_id, NULL, start_thread, NULL);

	// Main gtk loop
	gtk_main();
	gdk_threads_leave();

	return 0;
}
