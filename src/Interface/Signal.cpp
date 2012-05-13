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
	gchar * group = NULL;
	if (gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_info->entry))) {
		group = strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_info->entry)));
	}

	// Delete group
	if (!group || !current_client->remove_group(group)) {
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
 * Test if phone number is correct
 */
static bool phone_is_number(string phone) {
	if (phone.length() <= 10) {
		return false;
	}
	for (string::iterator c = phone.begin(); c != phone.end(); c++) {
		if (*c < '0' || *c > '9') {
			return false;
		}
	}
	return true;
}

/**
 * Update user's profile
 */
void signal_update_profile(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

	// Save profile information
	gchar * name = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->profile.name)));
	gchar * surname = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->profile.surname)));
	gchar * phone = strdup(gtk_entry_get_text(GTK_ENTRY(g_info->profile.phone)));
	GtkTextBuffer * buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(g_info->profile.hobbies));
	GtkTextIter start, end;
	gtk_text_buffer_get_bounds(buffer, &start, &end);
	gchar * hobbies = gtk_text_buffer_get_text(buffer, &start, &end, FALSE);

	// Verify profile information
	if (!phone_is_number(phone) || strlen(hobbies) >= BUFFER_LENGTH / 10) {
		clientgtk_create_message_dialog("Invalid phone / hobbies", "Error", GTK_MESSAGE_ERROR);

		// Free space
		free(name); free(surname); free(phone); free(hobbies);
		return;
	}

	// Add friend to user list TODO
//	if (!current_client->update_profile(name, surname, phone, "", hobbies)) { // TODO email
//		clientgtk_create_message_dialog("Invalid profile", "Warning", GTK_MESSAGE_WARNING);
//
//		// Free space
//		free(name); free(surname); free(phone); free(hobbies);
//		return;
//	}

	// Destroy windows
	gtk_widget_destroy(g_info->window);
	gtk_widget_destroy(g_info->vbox_align);

	// Free space
	free(name); free(surname); free(phone); free(hobbies);
	free(g_info);

	// Remake main interface
	clientgtk_create_main_window(window_top_level);
}

/**
 * Change user's group
 */
void signal_change_group(struct _general_info * g_info) {
	GtkWidget * window_top_level = (GtkWidget *) g_info->window_top_level;

	// Save friend's username
	gchar * group = NULL;
	if (gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_info->entry))) {
		group = strdup(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(g_info->entry)));
	}

	// Delete group
	cout << g_info->client << " " << group << endl << flush;
	if (!group || !current_client->move_user_to_group(g_info->client, group)) {
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
