/*
 * ClientGTKFunction.cpp
 *
 *  Created on: Apr 7, 2012
 *      Author: mihailc
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <map>

#include <gtk/gtk.h>

#include "ClientGTKFunctions.h"

#define CHAT_WINDOW_WIDTH	400
#define CHAT_WINDOW_HEIGHT	400

using namespace std;

// Map used for correspondence between client - chat window
map <string, GtkWidget *> map_chat_windows;

/**
 * Add a entry/button to a vbox and center it
 */
static void create_vbox_row(GtkWidget * vbox, GtkWidget * entry, gint width, gint height) {
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
 * Create a login link button
 */
static void create_login_link_button(GtkWidget * vbox, string text) {
	GtkWidget * recovery_button = gtk_button_new();
	gtk_button_set_relief(GTK_BUTTON(recovery_button), GTK_RELIEF_NONE);
	create_vbox_row(vbox, recovery_button, 0, 0);

	GtkWidget * recovery_label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(recovery_label),
			("<span foreground=\"orange\"><u>" + text + "</u></span>").c_str());
	gtk_container_add(GTK_CONTAINER(recovery_button), recovery_label);
	gtk_widget_show(recovery_label);
}

/**
 * Create a vbox used for login interface
 */
void clientgtk_create_login_vbox(GtkWidget * window_top_level) {
	// Create vbox and it's alignment
	GtkWidget * login_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	gtk_container_add(GTK_CONTAINER(window_top_level), login_vbox_align);
	gtk_widget_show(login_vbox_align);
	GtkWidget * login_vbox = gtk_vbox_new(FALSE, 5);
	gtk_container_add(GTK_CONTAINER(login_vbox_align), login_vbox);
	gtk_widget_show(login_vbox);

	// Create username field and add it to the vbox TODO limit nr chars
	GtkWidget * username_label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(username_label), "<big>Enter username:</big>");
	gtk_box_pack_start(GTK_BOX(login_vbox), username_label, FALSE, FALSE, 0);
	gtk_widget_show(username_label);
	GtkWidget * username_entry = gtk_entry_new();
	create_vbox_row(login_vbox, username_entry, WINDOW_WIDTH / 2, 0);

	// Create password field and add it to the vbox TODO limit nr chars
	GtkWidget * password_label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(password_label), "<big>Enter password:</big>");
	gtk_box_pack_start(GTK_BOX(login_vbox), password_label, FALSE, FALSE, 0);
	gtk_widget_show(password_label);
	GtkWidget * password_entry = gtk_entry_new();
	gtk_entry_set_visibility((GtkEntry *) password_entry, FALSE);
	create_vbox_row(login_vbox, password_entry, WINDOW_WIDTH / 2, 0);

	// Create login button
	GtkWidget * login_button = gtk_button_new_with_label("Login");
	create_vbox_row(login_vbox, login_button, 0, 0);

	// Action on login
	struct _login_info * l_info = (struct _login_info *) malloc(sizeof(struct _login_info));
	l_info->window_top_level = window_top_level;
	l_info->login_vbox_align = login_vbox_align;
	l_info->username_entry = username_entry;
	l_info->password_entry = password_entry;
	g_signal_connect_swapped(login_button, "clicked", G_CALLBACK(check_login), l_info);

	// Create account/password recovery button
	create_login_link_button(login_vbox, "Forgot account/password?");

	// Create  new account button
	create_login_link_button(login_vbox, "New account");
}

/**
 * Check number of clicks in order to create/focus a chat window
 */
static void check_friend_clicks(GtkWidget *widget, GdkEventButton *event, gpointer g_client) {
	// Check number of clicks
	if (GTK_IS_BUTTON(widget) &&
			(event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)) {

		// Check if chat window already exists
		char * client = (char *) g_client;
		map <string, GtkWidget *>::iterator it;
		it = map_chat_windows.find(client);

		printf("%s\n", (client)); // TODO delete

		// Create new chat window and add client to map
		if (it == map_chat_windows.end()) {
			clientgtk_create_chat_window(g_client);
		} else {
			gtk_widget_grab_focus(GTK_WIDGET(it->second));
			// TODO
		}
	}
}

/**
 * Create main interface which contains friends // TODO change string
 */
void clientgtk_create_main_vbox(GtkWidget * window_top_level) {
	// Create scroll window
	GtkWidget * scroll_window = gtk_scrolled_window_new(NULL, NULL);
	gtk_container_add(GTK_CONTAINER(window_top_level), scroll_window);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll_window),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_widget_show(scroll_window);

	// Create event box and set it's background color
	GtkWidget * event_box = gtk_event_box_new();
	GdkColor event_color;
	gdk_color_parse(WINDOW_BG_COLOR, &event_color);
	gtk_widget_modify_bg(event_box, (GtkStateType) GTK_STATE_NORMAL, &event_color);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scroll_window), event_box);
	gtk_widget_show(event_box);

	// Create vbox
	GtkWidget * main_vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(event_box), main_vbox);
	gtk_widget_show(main_vbox);

	// Example TODO change
	GtkWidget * friend_button, *friend_label;
	for (int i = 0; i < 20; i++) {
		// Friend button
		friend_button = gtk_button_new();
		gtk_button_set_relief(GTK_BUTTON(friend_button), GTK_RELIEF_NONE);
		gtk_button_get_focus_on_click(GTK_BUTTON(friend_button));
		gtk_box_pack_start(GTK_BOX(main_vbox), friend_button, FALSE, FALSE, 0);
		gtk_widget_show(friend_button);

		// Action on click // TODO change param
		char * client = (char *) malloc(strlen("Client: ") + 4 + 1); // TODO change 4
		sprintf(client, "Client: %i", i);
		g_signal_connect(friend_button, "button_press_event",
				G_CALLBACK(check_friend_clicks), (gpointer) client);

		// TODO free space occupied by client -> might not be necessart

		// Friend label and it's alignment
		GtkWidget * friend_align = gtk_alignment_new(0, 0.5, 0, 0);
		gtk_container_add(GTK_CONTAINER(friend_button), friend_align);
		gtk_widget_show(friend_align);
		friend_label = gtk_label_new("");
		gtk_label_set_markup(GTK_LABEL(friend_label), // TODO change test string
				("<big><big>" + string(client) + "</big></big>").c_str());
		gtk_container_add(GTK_CONTAINER(friend_align), friend_label);
		gtk_widget_show(friend_label);
	}
}

/**
 * Destroy a chat window
 */
inline static void destroy_chat_window(GtkWidget * chat_window, gpointer g_client) {
	char * client = (char *) g_client;
	map_chat_windows.erase(client);
	gtk_widget_destroy(chat_window);
}

/**
 * Create chat text in a scrolled window
 */
static GtkWidget * scrolled_chat_text_view(GtkWidget * vbox, gboolean from_start) {
	// Scrolled window for text view
	GtkWidget * swindow = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swindow),
			GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	if (from_start == TRUE) {
		gtk_box_pack_start(GTK_BOX(vbox), swindow, TRUE, TRUE, 0);
	} else {
		gtk_box_pack_end(GTK_BOX(vbox), swindow, FALSE, FALSE, 0);
	}
	gtk_widget_show(swindow);

	// Create text view
	GtkWidget * text_view = gtk_text_view_new();
	gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_view), GTK_WRAP_WORD_CHAR);
	gtk_container_add(GTK_CONTAINER(swindow), text_view);

	return text_view;
}

/**
 * Create a chat window // TODO change string parameter
 */
void clientgtk_create_chat_window(gpointer g_client) {
	char * client = (char *) g_client;

	// Main window
	GtkWidget * chat_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(chat_window), client);
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
	gtk_widget_show(conversation_chat);

	//////////// TODO make function //////////
	// Send button
	GtkWidget * send_button = gtk_button_new();
	//gtk_button_set_relief(GTK_BUTTON(send_button), GTK_RELIEF_NONE);
	gtk_button_get_focus_on_click(GTK_BUTTON(send_button));
	gtk_box_pack_start(GTK_BOX(chat_vbox), send_button, FALSE, FALSE, 0);
	gtk_widget_show(send_button);

	// Send button label and it's alignment
	GtkWidget * send_align = gtk_alignment_new(1, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(send_button), send_align);
	gtk_widget_show(send_align);
	GtkWidget * send_label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(send_label), "<big>Send</big>");
	gtk_container_add(GTK_CONTAINER(send_align), send_label);
	gtk_widget_show(send_label);
	//////////// TODO make function //////////

	// Create text view used for input
	GtkWidget * entry_chat = scrolled_chat_text_view(chat_vbox, FALSE);
	gtk_widget_show(entry_chat);
	g_signal_connect(entry_chat, "key-press-event",
			G_CALLBACK(send_text), (gpointer) conversation_chat);

	// Add chat window to map
	map_chat_windows.insert(pair <string, GtkWidget *> (client, chat_window));

	// Signal to kill main gtk loop
	g_signal_connect(chat_window, "destroy", G_CALLBACK(destroy_chat_window), client);
}
