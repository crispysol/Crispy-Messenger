/*
 * LoginWindow.cpp
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

#include "ClientGTK.h"

using namespace std;

// Map used for correspondence between client - chat window
extern map <string, GtkWidget *> map_chat_windows;

/**
 * Create an aligned vbox
 */
static GtkWidget * create_aligned_vbox(GtkWidget * window_top_level, GtkWidget * vbox_align) {
	gtk_container_add(GTK_CONTAINER(window_top_level), vbox_align);
	gtk_widget_show(vbox_align);
	GtkWidget * vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(vbox_align), vbox);
	gtk_widget_show(vbox);

	return vbox;
}

/**
 * Add a entry/button to a vbox and center it
 */
static void add_vbox_row(GtkWidget * vbox, GtkWidget * entry, gint width, gint height) {
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
 * Create a label field
 */
static void create_label_field(GtkWidget * vbox, string label_text) {
	GtkWidget * label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label), ("<big>" + label_text + "</big>").c_str());
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
}

/**
 * Create a login link button
 */
static GtkWidget * create_link_button(GtkWidget * vbox, string text) {
	GtkWidget * button = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	add_vbox_row(vbox, button, 0, 0);

	GtkWidget * label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label),
			("<span foreground=\"orange\"><u>" + text + "</u></span>").c_str());
	gtk_container_add(GTK_CONTAINER(button), label);
	gtk_widget_show(label);

	return button;
}


/**
 * Create a vbox used for login interface
 */
void clientgtk_create_login_window(GtkWidget * window_top_level) {
	// Create vbox and it's alignment
	GtkWidget * login_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	GtkWidget * login_vbox = create_aligned_vbox(window_top_level, login_vbox_align);

	// Create username field and add it to the vbox TODO limit nr chars
	create_label_field(login_vbox, string("Enter username:"));
	GtkWidget * username_entry = gtk_entry_new();
	add_vbox_row(login_vbox, username_entry, WINDOW_WIDTH / 2, 0);

	// Create password field and add it to the vbox TODO limit nr chars
	create_label_field(login_vbox, string("Enter password:"));
	GtkWidget * password_entry = gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry *) password_entry, FALSE);
	add_vbox_row(login_vbox, password_entry, WINDOW_WIDTH / 2, 0);

	// Create login button
	GtkWidget * login_button = gtk_button_new_with_label("Login");
	add_vbox_row(login_vbox, login_button, 0, 0);

	// Action on login
	struct _general_info * g_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	g_info->window_top_level = window_top_level;
	g_info->vbox_align = login_vbox_align;
	g_info->username = username_entry;
	g_info->password1 = password_entry;
	g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(signal_check_login), g_info);

	// Create account/password recovery button
	GtkWidget * recovery_button = create_link_button(login_vbox, "Forgot account/password?");
	g_signal_connect_swapped(recovery_button, "clicked",
			G_CALLBACK(client_gtk_create_recovery_window), g_info);

	// Create new account button
	GtkWidget * new_account_button = create_link_button(login_vbox, "New account");
	g_signal_connect_swapped(new_account_button, "clicked",
			G_CALLBACK(client_gtk_create_new_account_window), g_info);
}

/**
 * Create recovery window
 */
void client_gtk_create_recovery_window(struct _general_info * g_info) {
	// Save window top level
	GtkWidget * window_top_level = g_info->window_top_level;

	// Delete login interface
	gtk_widget_destroy(g_info->vbox_align);

	// Free space used by login interface
	free(g_info);

	// Create vbox and it's alignment
	GtkWidget * recovery_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	GtkWidget * recovery_vbox = create_aligned_vbox(window_top_level, recovery_vbox_align);

	// Create userneame field TODO limit nr chars
	create_label_field(recovery_vbox, string("Enter email used for registration:"));
	GtkWidget * email_entry = gtk_entry_new();
	add_vbox_row(recovery_vbox, email_entry, WINDOW_WIDTH / 2, 0);

	// Create recovery button
	GtkWidget * recovery_button = gtk_button_new_with_label("Register");
	add_vbox_row(recovery_vbox, recovery_button, 0, 0);

	// Action on recovery
	struct _general_info * ng_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	ng_info->window_top_level = window_top_level;
	ng_info->vbox_align = recovery_vbox_align;
	ng_info->email = email_entry;
	g_signal_connect_swapped(recovery_button, "clicked", G_CALLBACK(signal_check_recovery), ng_info);
}

/**
 * Create window for new account details
 */
void client_gtk_create_new_account_window(struct _general_info * g_info) {
	// Save window top level
	GtkWidget * window_top_level = g_info->window_top_level;

	// Delete login interface
	gtk_widget_destroy(g_info->vbox_align);

	// Free space used by login interface
	free(g_info);

	// Create vbox and it's alignment
	GtkWidget * new_account_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	GtkWidget * new_account_vbox = create_aligned_vbox(window_top_level, new_account_vbox_align);

	// Create userneame field TODO limit nr chars
	create_label_field(new_account_vbox, string("Enter username:"));
	GtkWidget * username_entry = gtk_entry_new();
	add_vbox_row(new_account_vbox, username_entry, WINDOW_WIDTH / 2, 0);

	// Create first password field TODO limit nr chars
	create_label_field(new_account_vbox, string("Enter password:"));
	GtkWidget * password_entry1 = gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry *) password_entry1, FALSE);
	add_vbox_row(new_account_vbox, password_entry1, WINDOW_WIDTH / 2, 0);

	// Create second password field TODO limit nr chars
	create_label_field(new_account_vbox, string("Retype password:"));
	GtkWidget * password_entry2 = gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry *) password_entry2, FALSE);
	add_vbox_row(new_account_vbox, password_entry2, WINDOW_WIDTH / 2, 0);

	// Create email field
	create_label_field(new_account_vbox, string("Enter email:"));
	GtkWidget * email_entry = gtk_entry_new();
	add_vbox_row(new_account_vbox, email_entry, WINDOW_WIDTH / 2, 0);

	// Create register button
	GtkWidget * register_button = gtk_button_new_with_label("Register");
	add_vbox_row(new_account_vbox, register_button, 0, 0);

	// Action on create account
	struct _general_info * ng_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	ng_info->window_top_level = window_top_level;
	ng_info->vbox_align = new_account_vbox_align;
	ng_info->username = username_entry;
	ng_info->password1 = password_entry1;
	ng_info->password2 = password_entry2;
	ng_info->email = email_entry;
	g_signal_connect_swapped(register_button, "clicked",
			G_CALLBACK(signal_check_register), ng_info);
}
