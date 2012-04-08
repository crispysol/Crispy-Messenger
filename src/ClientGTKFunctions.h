/*
 * ClientGTKFunction.h
 *
 *  Created on: Apr 7, 2012
 *      Author: mihailc
 */

#ifndef CLIENTGTKFUNCTION_H_
#define CLIENTGTKFUNCTION_H_

#define WINDOW_TITLE	"Crispy Messenger"
#define WINDOW_WIDTH	350
#define WINDOW_HEIGHT	600
#define WINDOW_BG_COLOR	"white"

/**
 * Structure used to pass login information
 */
struct _login_info {
	GtkWidget * window_top_level;
	GtkWidget * login_vbox_align;
	GtkWidget * username_entry;
	GtkWidget * password_entry;
};

/**
 * Create a vbox used for login interface
 */
void clientgtk_create_login_vbox(GtkWidget * window_top_level);

/**
 * Create main interface which contains friends
 */
void clientgtk_create_main_vbox(GtkWidget * window_top_level);

/**
 * On double click or triple click create a chat window
 */
void clientgtk_create_chat_window(gpointer func_data);

/**
 * Check login values and execute create main interface if everything is ok
 */
void check_login(struct _login_info * l_info);

/**
 * Send text to friend and save it in conversation text view
 */
gboolean send_text(GtkWidget * entry_chat, GdkEventKey * event, gpointer g_conversation_chat);

#endif /* CLIENTGTKFUNCTION_H_ */
