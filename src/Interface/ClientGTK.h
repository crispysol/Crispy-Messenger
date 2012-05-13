/*
 * ClientGTK.h
 *
 *  Created on: Apr 11, 2012
 *      Author: mihailc
 */

#ifndef CLIENTGTK_H_
#define CLIENTGTK_H_

#define WINDOW_TITLE			"Crispy Messenger"
#define WINDOW_WIDTH			350
#define WINDOW_HEIGHT			600
#define WINDOW_BG_COLOR			"white"

#define CHAT_WINDOW_WIDTH		300
#define CHAT_WINDOW_HEIGHT		400
#define AUX_WINDOW_WIDTH		200
#define AUX_WINDOW_HEIGHT		200
#define PROFILE_WINDOW_WIDTH	300
#define PROFILE_WINDOW_HEIGHT	400

#define MIN_REGISTER_CHARS		4
#define MAX_REGISTER_CHARS		30
#define MAX_EMAIL_CHARS			50
#define MAX_STATUS_CHARS		100
#define MAX_PROFILE_CHARS		15


/**
 * Structure used to pass login information
 */
struct _general_info {
	GtkWidget * window_top_level;
	GtkWidget * window;
	GtkWidget * vbox_align;
	GtkWidget * username;
	GtkWidget * password1;
	GtkWidget * password2;
	GtkWidget * email;
	GtkWidget * entry;
	const char * client;
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
 * Send text to friend and save it in conversation text view
 */
gboolean signal_send_text(GtkWidget * entry_chat, GdkEventKey * event, gpointer g_conversation_chat);

/**
 * Logout
 */
void signal_logout(GtkWidget * widget, gpointer info);

/**
 * Add a new friend
 */
void signal_add_friend(struct _general_info * g_info);

/**
 * Create a new group
 */
void signal_create_group(struct _general_info * g_info);

/**
 * Delete an existing group
 */
void signal_delete_group(struct _general_info * g_info);

/**
 * Change status
 */
void signal_change_status(struct _general_info * g_info);

/**
 * Change availability
 */
void signal_change_availability(struct _general_info * g_info);

/**
 * Show user's profile
 */
void signal_show_profile(GtkWidget * widget, gpointer g_client);

/**
 * Update user's profile
 */
void signal_update_profile(GtkWidget * widget, gpointer g_client);

/**
 * Change user's group
 */
void signal_change_group(GtkWidget * widget, gpointer g_client);

/**
 * Remove user from group
 */
void signal_remove_user(GtkWidget * widget, gpointer info);

#endif /* CLIENTGTK_H_ */
