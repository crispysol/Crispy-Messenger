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

#include "ClientGTK.h"

using namespace std;

// Map used for correspondence between client - chat window
extern map <string, GtkWidget *> map_chat_windows;

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
 * Create buttons for a chat window TODO
 */
static void create_chat_window_buttons(GtkWidget * vbox) {
	// Create button
	GtkWidget * send_button = gtk_button_new();
	gtk_button_get_focus_on_click(GTK_BUTTON(send_button));
	gtk_box_pack_start(GTK_BOX(vbox), send_button, FALSE, FALSE, 0);
	gtk_widget_show(send_button);

	// Send button label and it's alignment
	GtkWidget * send_align = gtk_alignment_new(1, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(send_button), send_align);
	gtk_widget_show(send_align);
	GtkWidget * send_label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(send_label), "<big>Send</big>");
	gtk_container_add(GTK_CONTAINER(send_align), send_label);
	gtk_widget_show(send_label);
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
 * Create a chat window // TODO change string parameter
 */
void clientgtk_create_chat_window(GtkWidget * widget, gpointer g_client) {
	char * client = (char *) g_client;
	printf("%s\n", (char *) g_client); // TODO delete

	// Chat window
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
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(conversation_chat), FALSE);
	gtk_widget_show(conversation_chat);

	// Create buttons
	create_chat_window_buttons(chat_vbox);

	// Create text view used for input
	GtkWidget * entry_chat = scrolled_chat_text_view(chat_vbox, FALSE);
	gtk_widget_grab_focus(entry_chat);
	gtk_widget_show(entry_chat);
	g_signal_connect(entry_chat, "key-press-event",
			G_CALLBACK(signal_send_text), (gpointer) conversation_chat);

	// Add chat window to map
	map_chat_windows.insert(pair <string, GtkWidget *> (client, chat_window));

	// Signal to kill window
	g_signal_connect(chat_window, "destroy", G_CALLBACK(destroy_chat_window), client);
}
