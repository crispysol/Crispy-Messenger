/*
 * SignalFunctions2.cpp
 *
 *  Created on: May 11, 2012
 *      Author: mihailc
 */

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>

#include <gtk/gtk.h>

#include "../Client.h"
#include "ClientGTK.h"

// Client info
extern Client * current_client;

/**
 * Add a new friend
 */
void signal_add_friend(struct _general_info * g_info) {
	GtkWidget * window = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// Add friend to user list TODO
//	if (!current_client->add_user(username)) {
//		clientgtk_create_message_dialog("Invalid username", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(username);
//
//		return;
//	}

	// Remake main window TODO

	// Free space
	free(username);
	free(g_info);

	// Destroy window
	gtk_widget_destroy(window);
}

/**
 * Create a new group
 */
void signal_create_group(struct _general_info * g_info) {
	GtkWidget * window = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// TODO
//	if (!current_client->add_user(username)) {
//		clientgtk_create_message_dialog("Invalid group", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(group);
//
//		return;
//	}

	// Remake main window TODO

	// Free space
	free(username);
	free(g_info);

	// Destroy window
	gtk_widget_destroy(window);
}

/**
 * Delete an existing group
 */
void signal_delete_group(struct _general_info * g_info) {
	GtkWidget * window = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * group = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// TODO
//	if (!current_client->add_user(group)) {
//		clientgtk_create_message_dialog("Invalid group", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(group);
//
//		return;
//	}

	// Remake main window TODO

	// Free space
	free(group);
	free(g_info);

	// Destroy window
	gtk_widget_destroy(window);
}

/**
 * Delete an existing group
 */
void signal_change_status(struct _general_info * g_info) {
	GtkWidget * window = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * status = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// TODO
//	if (!current_client->add_user(status)) {
//		clientgtk_create_message_dialog("Invalid status", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(status);
//
//		return;
//	}

	// Remake main window TODO

	// Free space
	free(status);
	free(g_info);

	// Destroy window
	gtk_widget_destroy(window);
}

/**
 * Change availability
 */
void signal_change_availability(struct _general_info * g_info) {
	GtkWidget * window = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * state = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// TODO
//	if (!current_client->add_user(state)) {
//		clientgtk_create_message_dialog("Invalid availability", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(state);
//
//		return;
//	}

	// Remake main window TODO

	// Free space
	free(state);
	free(g_info);

	// Destroy window
	gtk_widget_destroy(window);
}

/**
 * Show user profile
 */
void signal_show_profile(GtkWidget * widget, gpointer g_client) {
	// TODO
}

/**
 * Move user to group
 */
void signal_move_to_group(GtkWidget * widget, gpointer g_client) {
	// TODO
}
