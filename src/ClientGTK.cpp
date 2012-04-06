/*
 * ClientGTK.cpp
 *
 *  Created on: Mar 13, 2012
 *      Author: mihail
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>

#include <gtk/gtk.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "ServerFunctions.h"
#include "Client.h"

// Defines
#define BUFFER_LENGTH	256
#define WINDOW_TITLE	"Crispy Messenger"
#define WINDOW_WIDTH	350
#define WINDOW_HEIGHT	600
#define WINDOW_BG_COLOR	"white"

using namespace std;

// Local server info
static int sockfd;
static fd_set read_fds;
static fd_set tmp_fds;
static int fdmax;
// Server socket
static int socket_server;

// Top level window
GtkWidget * window_top_level;

// TODO
int check = 0;

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
				cout << "TODO" << endl; //
			}
		}
	}
}

/**
 * Add a entry/button to a vbox and center it
 */
static void create_vbox_entry(GtkWidget * vbox, GtkWidget * entry, gint width, gint height) {
	GtkWidget * align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_box_pack_start(GTK_BOX(vbox), align, FALSE, FALSE, 0);
	gtk_widget_show(align);

	GtkWidget * hbox = gtk_hbox_new(TRUE, 0);
	gtk_container_add(GTK_CONTAINER(align), hbox);
	gtk_widget_show(hbox);

	gtk_widget_set_size_request(entry, width, height);
	gtk_container_add(GTK_CONTAINER(hbox), entry);
	gtk_widget_show(entry);
}

/**
 * Create main interface which contains friends
 */
static void create_main_vbox(GtkWidget * login_vbox_align) {
	gtk_widget_destroy(login_vbox_align);
	// Retrieve users // TODO

	// Create vbox
	GtkWidget * main_vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(window_top_level), main_vbox);
	gtk_widget_show(main_vbox);

	// Example TODO delete
	GtkWidget * button;
	for (int i = 0; i < 20; i++) {
		button = gtk_button_new_with_label("Test");
		//g_signal_connect(button, "clicked", G_CALLBACK(test), NULL); // TODO change function
		//gtk_widget_set_size_request(button, WINDOW_WIDTH, 50); // TODO
		gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
		//gtk_box_pack_start(GTK_BOX(main_vbox), button, FALSE, FALSE, 0);
		create_vbox_entry(main_vbox, button, WINDOW_WIDTH, 50);
	}
}

/**
 * Create a vbox used for login interface
 */
static void create_login_vbox() {
    // Create vbox and it's alignment
	GtkWidget * login_vbox_align = gtk_alignment_new(0.5, 0.25, 0, 0);
	gtk_container_add(GTK_CONTAINER(window_top_level), login_vbox_align);
	gtk_widget_show(login_vbox_align);
	GtkWidget * login_vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(login_vbox_align), login_vbox);
	gtk_widget_show(login_vbox);

	// Create username field and add it to the vbox
	GtkWidget * username_label = gtk_label_new("Enter username:");
	gtk_box_pack_start(GTK_BOX(login_vbox), username_label, FALSE, FALSE, 0);
	gtk_widget_show(username_label);
	GtkWidget * username_entry = gtk_entry_new();
	create_vbox_entry(login_vbox, username_entry, WINDOW_WIDTH / 3, 0);

	// Create password field and add it to the vbox
	GtkWidget * password_label = gtk_label_new("Enter password:");
	gtk_box_pack_start(GTK_BOX(login_vbox), password_label, FALSE, FALSE, 0);
	gtk_widget_show(password_label);
	GtkWidget * password_entry = gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry *) password_entry, FALSE);
	create_vbox_entry(login_vbox, password_entry, WINDOW_WIDTH / 3, 0);

	// Create login button
	GtkWidget * login_button = gtk_button_new_with_label("Login");
	create_vbox_entry(login_vbox, login_button, 0, 0);

	// Action on login
	g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(create_main_vbox), login_vbox_align);

	// Create account/password recovery button and new account button
	GtkWidget * recovery_button = gtk_button_new_with_label("Forgot account/password?");
	gtk_button_set_relief(GTK_BUTTON(recovery_button), GTK_RELIEF_NONE);
	create_vbox_entry(login_vbox, recovery_button, 0, 0);
	GtkWidget * new_account_button = gtk_button_new_with_label("New account");
	gtk_button_set_relief(GTK_BUTTON(new_account_button), GTK_RELIEF_NONE);
	create_vbox_entry(login_vbox, new_account_button, 0, 0);


}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
	// Check if number of arguments is correct
	if (argc != 3) {
		cerr << "Usage: ./server.cpp server_ip port" << endl;
		exit(EXIT_FAILURE);
	}

	// Init GTK
	gtk_init(&argc, &argv);

	// Create main window
	window_top_level = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window_top_level), WINDOW_TITLE);
	gtk_window_set_default_size(GTK_WINDOW(window_top_level), WINDOW_WIDTH, WINDOW_HEIGHT);
	//gtk_window_set_position(GTK_WINDOW(window_top_level), GTK_WIN_POS_CENTER); TODO uncomment

	// Set main window bg color
	GdkRGBA color;
	gdk_rgba_parse(&color, WINDOW_BG_COLOR);
	gtk_widget_override_background_color(window_top_level, (GtkStateFlags) GTK_STATE_FLAG_NORMAL, &color);
	gtk_widget_show(window_top_level);

	// Signal to kill main gtk loop
	g_signal_connect(window_top_level, "destroy", G_CALLBACK(gtk_main_quit), NULL);

	// Create login interface
	create_login_vbox();


    /********************************************************/
//	// Create buttons TODO modify
//	GtkWidget * button;
//	for (int i = 0; i < 4; i++) {
//		button = gtk_button_new_with_label("TEST");
//		g_signal_connect(button, "clicked", G_CALLBACK(test), NULL); // TODO change function
//		gtk_widget_set_size_request(button, WINDOW_WIDTH, 50); // TODO
//		gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
//		gtk_table_attach(GTK_TABLE(table), button, 0, 1, i, i + 1,
//				(GtkAttachOptions) (GTK_FILL | GTK_SHRINK),
//				(GtkAttachOptions) (GTK_FILL | GTK_SHRINK), 1, 1);
//	}
//
//	// Add idle function
//	Settings settings; // TODO create settings for gtk buttons
//	settings.button = button;
//	/g_idle_add((GSourceFunc) test_idle, 0); // TODO change function


	/********************************************************/

	// Init localhost server (communication with other clients
	//init_server(socket_server, sockfd, fdmax, &read_fds); TODO

	// Connect to main server
	//connect_to_server(argv[1], atoi(argv[2]), socket_server, fdmax, &read_fds); TODO

	// Main gtk loop
	gtk_main();

	return 0;
}
