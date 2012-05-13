/*
 * ChatWindow.cpp
 *
 *  Created on: Apr 30, 2012
 *      Author: mihailc
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <gtk/gtk.h>

#include "GTKFunctions.h"
#include "ClientGTK.h"

using namespace std;

// Map used for correspondence between client - chat window
extern map <string, GtkWidget *> map_chat_windows;
extern map <string, GtkWidget *> map_chat_text;

/* Send a file */
static void send_file(GtkWidget * widget, GdkEventButton * event, gpointer g_client) {
	signal_send_file(widget, g_client);
}
///* Show profile */ TODO
//static void show_profile(GtkWidget * widget, GdkEventButton * event, gpointer g_client) {
//	signal_show_profile(widget, g_client);
//}
///* Show profile */
//static void change_group(GtkWidget * widget, GdkEventButton * event, gpointer g_client) {
//	signal_change_group(widget, g_client);
//}


/**
 * Create buttons for a chat window
 */
static void create_chat_window_buttons(GtkWidget * vbox, gpointer g_client) {
	// Create hbox
	GtkWidget * hbox = gtk_hbox_new(FALSE, 0);
	gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

	// Create send button
	add_button_to_box(hbox, "Send file", TRUE, send_file, g_client);
//	// Create show profile button TODO
//	add_button_to_box(hbox, "Show profile", FALSE, show_profile, g_client);
//	// Create change group button
//	add_button_to_box(hbox, "Change group", FALSE, change_group, g_client);

	// Show all widgets
	gtk_widget_show_all(hbox);
}

/**
 * Destroy a chat window
 */
inline static void destroy_chat_window(GtkWidget * chat_window, struct _general_info * ng_info) {
//	gdk_threads_enter();
	map_chat_windows.erase(ng_info->friend_username);
	map_chat_text.erase(ng_info->friend_username);
	gtk_widget_destroy(chat_window);
	free(ng_info->friend_username);
	free(ng_info);
//	gdk_threads_leave();
}

/**
 * Create a chat window
 */
void clientgtk_create_chat_window(GtkWidget * widget, gpointer g_client) {
	char * friend_username = strdup((const char * ) g_client);

	// Chat window
	GtkWidget * chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(chat_window), friend_username);
	gtk_window_set_default_size(GTK_WINDOW(chat_window),
			CHAT_WINDOW_WIDTH, CHAT_WINDOW_HEIGHT);
	gtk_widget_show(chat_window);

	// Create vbox
	GtkWidget * chat_vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(chat_window), chat_vbox);
	gtk_widget_show(chat_vbox);

	// Create text view used for conversation
	GtkWidget * conversation_chat = scrolled_chat_text_view(chat_vbox, TRUE);
	gtk_text_view_set_editable(GTK_TEXT_VIEW(conversation_chat), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(conversation_chat), FALSE);
	gtk_widget_show(conversation_chat);

	// Create buttons
	create_chat_window_buttons(chat_vbox, g_client);

	// Create text view used for input
	GtkWidget * entry_chat = scrolled_chat_text_view(chat_vbox, FALSE);
	gtk_widget_grab_focus(entry_chat);
	gtk_widget_show(entry_chat);

	// Action on key press
	struct _general_info * ng_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	ng_info->window = conversation_chat;
	ng_info->friend_username = friend_username;
	g_signal_connect(entry_chat, "key-press-event",
			G_CALLBACK(signal_send_text), (gpointer) ng_info);

	// Add chat window to map
	map_chat_windows.insert(pair <string, GtkWidget *> (ng_info->friend_username, chat_window));
	map_chat_text.insert(pair <string, GtkWidget *> (ng_info->friend_username, conversation_chat));

	// Signal to kill window
	g_signal_connect(chat_window, "destroy", G_CALLBACK(destroy_chat_window), ng_info);
}
