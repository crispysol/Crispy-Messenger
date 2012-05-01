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

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms-compat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <regex.h>

#include "../ServerFunctions.h"
#include "../Client.h"
#include "ClientGTK.h"

// Defines
#define BUFFER_LENGTH	256
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
static int socket_server;

// Client info
Client * current_client;

// Map used for correspondence between client - chat window
map <string, GtkWidget *> map_chat_windows;

/**
 * Check login values and execute create main interface if everything is ok
 */
void signal_check_login(struct _general_info * g_info) {
	// Save window top level
	GtkWidget * window_top_level = g_info->window_top_level;

	// Save username and password
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->username)));
	gchar * password = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->password1)));

	// Retrieve users -> communicate with server // TODO
	cout << "username: " << username << endl << "password: " << password << endl;

	// Delete username and password
	free(username);
	free(password);

	// Delete login interface
	gtk_widget_destroy(g_info->vbox_align);

	// Free space used by login interface
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
		// Register client TODO must retrieve answer too
//		current_client->register_client(username, password1, email);

		// Show information about register // TODO check answer from register_client to see if everything is ok
		clientgtk_create_message_dialog("Account created", "Register information",
				GTK_MESSAGE_INFO);

		// Delete register interface
		gtk_widget_destroy(g_info->vbox_align);

		// Change interface back to login
		clientgtk_create_login_window(g_info->window_top_level);

		// Free space used by register interface
		free(g_info);
	}

	// Delete username and password
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
		// TODO function does not exist, must be created in server !!!

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

	// Free email space
	free(email);
}

/**
 * Select file for sending
 */
void signal_send_file(GtkWidget * widget, gpointer g_client) {
	GtkWidget * chooser = gtk_file_chooser_dialog_new("Choose file ...", NULL,
			GTK_FILE_CHOOSER_ACTION_OPEN, GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_OK, NULL);

	gchar * filename = NULL;
	if (gtk_dialog_run(GTK_DIALOG(chooser)) == GTK_RESPONSE_OK) {
		filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(chooser));
	}

	cout << "filename: " << filename << endl;

	// TODO send file

	gtk_widget_destroy(chooser);
}

/**
 * Send text to friend and save it in conversation text view // TODO
 */
gboolean signal_send_text(GtkWidget * entry_chat, GdkEventKey * event,
		gpointer g_conversation_chat) {
	if (event->keyval == GDK_Return || event->keyval == GDK_KP_Enter) {
		// Get text from input
		GtkTextBuffer * buffer1 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(entry_chat));
		GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer1, &start, &end);
		gchar * new_text = gtk_text_buffer_get_text(buffer1, &start, &end, FALSE);

		// Set text to conversation box
		GtkWidget * conversation_chat = (GtkWidget *) g_conversation_chat;
		GtkTextBuffer * buffer2 = gtk_text_view_get_buffer(GTK_TEXT_VIEW(conversation_chat));
		gtk_text_buffer_get_bounds(buffer2, &start, &end);
		gchar * old_text = gtk_text_buffer_get_text(buffer2, &start, &end, FALSE);
		string text;
		if (strcmp(old_text, "") == 0) {
			text = string(new_text);
		} else {
			text = string(old_text) + "\n" + string(new_text);
		}
		gtk_text_buffer_set_text(buffer2, text.c_str(), -1);

		// Free space
		free(old_text);
		free(new_text);

		// Delete all text from input
		gtk_text_buffer_set_text(buffer1, "", -1);
		return TRUE;
	}
	return FALSE;
}

/**
 * Function that runs in gtk main loop
 */
void idle(gpointer data) {
	// Run local server
	char buffer[BUFFER_LENGTH];
	int n;

	FD_ZERO(&tmp_fds);
	tmp_fds = read_fds;
	assert(select(fdmax + 1, &tmp_fds, NULL, NULL, NULL) != -1);

	// Check every socket for new data
	for (int i = 0; i <= fdmax; i++) {
		if (FD_ISSET(i, &tmp_fds)) {
			// New connection
			if (i == sockfd) {
				new_connection(sockfd, fdmax, &read_fds);
				continue;
			}

			// Execute command from STDIN
			if (i == STDIN_FILENO) {
				// stdin_command(); TODO might delete
				continue;
			}

			// Receive data from server
			if (i == socket_server) {
				// TODO if (i == server_port) => quit application and free all resource (sockets + etc)
				n = recv(i, buffer, sizeof(buffer), 0);
				assert(n >= 0); // TODO test if we have to exit

				cout << buffer << " | RECEIVED" << endl;
				continue;
			}

			// Received data from another client
			if ((n = recv(i, buffer, sizeof(buffer), 0)) <= 0) {
				assert(n == 0);
				end_connection(i, &read_fds);
				//TODO remove connection from connected_users list
			} else {
				//client_command(buffer, i, inet_ntoa(cli_addr.sin_addr),
				//	&database, result); TODO
				cout << "TODO" << endl;
			}
		}
	}
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
//	// Init localhost server (communication with other clients
//	init_server(socket_server, sockfd, fdmax, &read_fds); // TODO
//
//	// Connect to main server
//	connect_to_server(ip.c_str(), port, socket_server, fdmax, &read_fds); // TODO
//
//	// Create current client
//	current_client = new Client(socket_server, ip.c_str());

	// Init GTK
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

	// Add idle function
//	g_idle_add((GSourceFunc) idle, 0); // TODO change function

	// Main gtk loop
	gtk_main();

	return 0;
}
