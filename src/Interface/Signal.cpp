/*
 * SignalFunctions2.cpp
 *
 *  Created on: May 11, 2012
 *      Author: mihailc
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <string>

#include <gtk/gtk.h>

#include "../Client.h"
#include "ClientGTK.h"

// Client info
extern Client * current_client;

using namespace std;

/**
 * Add a new friend
 */
void signal_add_friend(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * username = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// Add friend to user list
	if (!current_client->add_user(username)) {
		clientgtk_create_message_dialog("Invalid username", "Warning", GTK_MESSAGE_WARNING);

		// Free space
		free(username);
		return;
	}

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(username);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Create a new group
 */
void signal_create_group(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * group = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->entry)));

	// Create new group
	if (!current_client->add_group(group)) {
		clientgtk_create_message_dialog("Invalid group", "Warning", GTK_MESSAGE_WARNING);

		// Free space
		free(group);
		return;
	}

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(group);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Delete an existing group
 */
void signal_delete_group(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

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

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(group);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Delete an existing group
 */
void signal_change_status(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

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

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(status);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Change availability
 */
void signal_change_availability(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

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

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(state);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Show user's profile
 */
void signal_show_profile(GtkWidget * widget, gpointer g_client) {
	// TODO
}

/**
 * Update user's profile
 */
void signal_update_profile(GtkWidget * widget, gpointer g_client) {
	// TODO
}

/**
 * Change user's group
 */
void signal_change_group(GtkWidget * widget, gpointer g_client) {
	// TODO
}

/**
 * Remove user from list
 */
void signal_remove_user(GtkWidget * widget, gpointer info) {
	struct _general_info * g_info = (struct _general_info *) info;
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	const char * username = g_info->client;

	// Add friend to user list
	if (!current_client->remove_user(username)) {
		clientgtk_create_message_dialog("Invalid username", "Warning", GTK_MESSAGE_WARNING);
		return;
	}

	// Destroy windows
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}
