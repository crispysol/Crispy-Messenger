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
#include "GTKFunctions.h"

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
	if (username == current_client->get_username() || !current_client->add_user(username)) {
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

	// Save group's name
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
void signal_show_profile(struct _general_info * g_info) {
	// Get profile
	Profile profile = current_client->get_profile(g_info->client);

	// Show profile
	gint width = PROFILE_WINDOW_WIDTH, height = PROFILE_WINDOW_HEIGHT;

	// Create new window
	GtkWidget * window = create_new_window(width, height, (gchar *) "Update profile");

	// Create vbox and it's alignment
	GtkWidget * new_vbox_align = gtk_alignment_new(0.5, 0.2, 0, 0);
	GtkWidget * new_vbox = create_aligned_vbox(window, new_vbox_align);

	// Create name entry
	create_label_field(new_vbox, string("Name"));
	GtkWidget * name = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(name), MAX_PROFILE_CHARS);
	gtk_entry_set_text(GTK_ENTRY(name), profile.get_name().c_str());
	gtk_editable_set_editable(GTK_EDITABLE(name), FALSE);
	add_vbox_row(new_vbox, name, width / 1.5, 0);

	// Create surname entry
	create_label_field(new_vbox, string("Surname"));
	GtkWidget * surname = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(surname), MAX_PROFILE_CHARS);
	gtk_entry_set_text(GTK_ENTRY(surname), profile.get_surname().c_str());
	gtk_editable_set_editable(GTK_EDITABLE(surname), FALSE);
	add_vbox_row(new_vbox, surname, width / 1.5, 0);

	// Create phone entry
	create_label_field(new_vbox, string("Phone"));
	GtkWidget * phone = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(phone), MAX_PROFILE_CHARS);
	gtk_entry_set_text(GTK_ENTRY(phone), profile.get_phone().c_str());
	gtk_editable_set_editable(GTK_EDITABLE(phone), FALSE);
	add_vbox_row(new_vbox, phone, width / 1.5, 0);

	// Create email entry
	create_label_field(new_vbox, string("Email"));
	GtkWidget * email = gtk_entry_new();
	gtk_entry_set_max_length(GTK_ENTRY(email), MAX_PROFILE_CHARS);
	gtk_entry_set_text(GTK_ENTRY(email), profile.get_email().c_str());
	gtk_editable_set_editable(GTK_EDITABLE(email), FALSE);
	add_vbox_row(new_vbox, email, width / 1.5, 0);

	// Create hobbies entry
	GtkWidget * swindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	GtkWidget * text_view = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
	gtk_container_add(GTK_CONTAINER(swindow), text_view);
	create_label_field(new_vbox, string("Hobbies"));
	add_vbox_row(new_vbox, swindow, width / 1.5, height / 3);
	GtkTextBuffer * buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
	gtk_text_buffer_set_text(buffer, profile.get_hobbies().c_str(), -1);

	// Show window
	gtk_widget_show_all(window);
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

	// Update profile
	if (!current_client->update_profile(name, surname, phone, hobbies)) {
		clientgtk_create_message_dialog("Invalid profile", "Warning", GTK_MESSAGE_WARNING);

		// Free space
		free(name); free(surname); free(phone); free(hobbies);
		return;
	}

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

	// Save group's name
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
