/*
 * ClientGTK.h
 *
 *  Created on: Apr 11, 2012
 *      Author: mihailc
 */

#ifndef CLIENTGTK_H_
#define CLIENTGTK_H_

#define WINDOW_TITLE		"Crispy Messenger"
#define WINDOW_WIDTH		350
#define WINDOW_HEIGHT		600
#define WINDOW_BG_COLOR		"white"
#define CHAT_WINDOW_WIDTH	400
#define CHAT_WINDOW_HEIGHT	400
#define MIN_REGISTER_CHARS	4
#define MAX_REGISTER_CHARS	30 // TODO add in function
#define MAX__EMAIL_CHARS	50 // TODO add in function

/**
 * Structure used to pass login information
 */
struct _general_info {
	GtkWidget * window_top_level;
	GtkWidget * vbox_align;
	GtkWidget * username;
	GtkWidget * password1;
	GtkWidget * password2;
	GtkWidget * email;
};

/**
 * Create a message dialog
 */
void clientgtk_create_message_dialog(const gchar * message, const gchar * title,
		GtkMessageType type);

/**
 * Create login interface
 */
void clientgtk_create_login_window(GtkWidget * window_top_level);

/**
 * Create recovery window
 */
void clientgtk_create_recovery_window(struct _general_info * g_info);

/**
 * Create window for new account details
 */
void clientgtk_create_new_account_window(struct _general_info * g_info);

/**
 * Create main interface which contains friends
 */
void clientgtk_create_main_window(GtkWidget * window_top_level);

/**
 * On double click or triple click create a chat window
 */
void clientgtk_create_chat_window(GtkWidget * widget, gpointer func_data);

/**
 * Check login values and execute create main interface if everything is ok
 */
void signal_check_login(struct _general_info * g_info);

/**
 * Check register values and register client
 */
void signal_check_register(struct _general_info * g_info);

/**
 * Check recovery values
 */
void signal_check_recovery(struct _general_info * g_info);

/**
 * Select file for sending
 */
void signal_send_file(GtkWidget * widget, gpointer g_client);

/**
 * Add a new friend
 */
void signal_add_friend(GtkWidget * widget);

/**
 * Send text to friend and save it in conversation text view
 */
gboolean signal_send_text(GtkWidget * entry_chat, GdkEventKey * event, gpointer g_conversation_chat);

#endif /* CLIENTGTK_H_ */
