/*
 * ClientGTKDraw.cpp
 *
 *  Created on: Apr 11, 2012
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
map <string, GtkWidget *> map_chat_windows;

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
	gtk_entry_set_visibility((GtkEntry *) email_entry, FALSE);
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

/////////////////////////////// TODO /////////////////////////////////////////////////////////////
/////////////////////////////// TODO /////////////////////////////////////////////////////////////

/**
 * TODO
 */
static void execute_menu_item(GtkWidget * widget, gpointer g_client) {
	printf("test\n");
}

/**
 * Create a entry for the context menu
 */
inline static void create_menu_entry(GtkWidget * menu, gchar * label_text,
		void (* handler)(GtkWidget *, gpointer), gpointer data) {
	GtkWidget * menu_item = gtk_menu_item_new_with_label(label_text);
	g_signal_connect(menu_item, "activate", (GCallback) handler, data);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show(menu_item);;
}

/**
 * Create a sub menu for a menu bar and return it
 */
inline static GtkWidget * create_menu_bar_submenu(GtkWidget * menu_bar, gchar * label_text) {
	GtkWidget * file_menu = gtk_menu_new();
	gtk_widget_show(file_menu);

	GtkWidget * entry = gtk_menu_item_new_with_label(label_text);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(entry), file_menu);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), entry);
	gtk_widget_show(entry);

	return file_menu;
}

/**
 * Check number of clicks in order to create/focus a chat window
 */
static void check_friend_clicks(GtkWidget * widget, GdkEventButton * event, gpointer g_client) {
	// Check number of left clicks
	if (GTK_IS_BUTTON(widget) && event->button == 1 &&
			(event->type == GDK_2BUTTON_PRESS || event->type == GDK_3BUTTON_PRESS)) {

		// Check if chat window already exists
		char * client = (char *) g_client;
		map <string, GtkWidget *>::iterator it;
		it = map_chat_windows.find(client);

		// Create new chat window and add client to map
		if (it == map_chat_windows.end()) {
			clientgtk_create_chat_window(NULL, g_client);
		} else {
			gtk_window_present(GTK_WINDOW(it->second));
		}
	}

	// Check if right click was pressed
	if (GTK_IS_BUTTON(widget) && event->button == 3) {
		// Create menu
		GtkWidget * context_menu = gtk_menu_new();
		gtk_widget_show(context_menu);

		// Create context menu entries TODO change functions
		create_menu_entry(context_menu, (gchar *) "Start chat",
				clientgtk_create_chat_window, g_client);
		create_menu_entry(context_menu, (gchar *) "Send file",
				execute_menu_item, g_client);
		create_menu_entry(context_menu, (gchar *) "Show profile",
				execute_menu_item, g_client);
		create_menu_entry(context_menu, (gchar *) "Move to group",
				execute_menu_item, g_client);

		// Create menu
		gtk_menu_popup(GTK_MENU(context_menu), NULL, NULL, NULL, NULL,
				(event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent *) event));
	}
}

/**
 * Create main interface which contains friends // TODO change string
 */
void clientgtk_create_main_window(GtkWidget * window_top_level) {
	// Create a vbox that contains the menu bar and the scrolled window
	GtkWidget * vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window_top_level), vbox);
	gtk_widget_show(vbox);

	// Create menu bar TODO change functions
	GtkWidget * menu_bar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
	gtk_widget_show(menu_bar);

	GtkWidget * submenu1 = create_menu_bar_submenu(menu_bar, (gchar *) "Friends");
	create_menu_entry(submenu1, (gchar *) "Add friend", execute_menu_item, NULL);
	create_menu_entry(submenu1, (gchar *) "Search friends", execute_menu_item, NULL);
	create_menu_entry(submenu1, (gchar *) "Create group", execute_menu_item, NULL);
	create_menu_entry(submenu1, (gchar *) "Delete group", execute_menu_item, NULL);

	GtkWidget * submenu2 = create_menu_bar_submenu(menu_bar, (gchar *) "Account");
	create_menu_entry(submenu2, (gchar *) "Change status", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Change availability", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Show profile", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Update profile", execute_menu_item, NULL);

	GtkWidget * submenu3 = create_menu_bar_submenu(menu_bar, (gchar *) "Settings");
	create_menu_entry(submenu3, (gchar *) "Change ip/port", execute_menu_item, NULL);

	// Create scroll window
	GtkWidget * scroll_window = gtk_scrolled_window_new(NULL, NULL);
	//gtk_container_add(GTK_CONTAINER(window_top_level), scroll_window);
	gtk_box_pack_start(GTK_BOX(vbox), scroll_window, TRUE, TRUE, 0);
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

	// Create main vbox
	GtkWidget * main_vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(event_box), main_vbox);
	gtk_widget_show(main_vbox);

	// Example TODO change
	GtkWidget * button, * label, * align;
	for (int i = 0; i < 2; i++) {
		// Group label TODO maybe function
		align = gtk_alignment_new(0.5, 0.5, 0, 0);
		gtk_box_pack_start(GTK_BOX(main_vbox), align, FALSE, FALSE, 0);
		gtk_widget_show(align);

		char * group = (char *) malloc(strlen("Group ") + 3); // TODO change 4
		sprintf(group, "Group %i", i);
		label = gtk_label_new("");
		gtk_label_set_markup(GTK_LABEL(label), // TODO change test string
				("<big><b><u>" + string(group) + "</u></b></big>").c_str());
		gtk_container_add(GTK_CONTAINER(align), label);
		gtk_widget_show(label);

		for (int j = 0; j < 10; j++) {
			// Friend button
			button = gtk_button_new();
			gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
			gtk_button_get_focus_on_click(GTK_BUTTON(button));
			gtk_box_pack_start(GTK_BOX(main_vbox), button, FALSE, FALSE, 0);
			gtk_widget_show(button);

			// Action on click // TODO change param
			char * client = (char *) malloc(strlen("Client ") + 5); // TODO change 4
			sprintf(client, "Client %i%i", i, j);
			g_signal_connect(button, "button_press_event",
					G_CALLBACK(check_friend_clicks), (gpointer) client);

			// TODO free space occupied by client -> might not be necessary

			// Friend label and it's alignment
			align = gtk_alignment_new(0, 0.5, 0, 0);
			gtk_container_add(GTK_CONTAINER(button), align);
			gtk_widget_show(align);
			label = gtk_label_new("");
			gtk_label_set_markup(GTK_LABEL(label), // TODO change test string
					("<big><big>" + string(client) + "</big></big>").c_str());
			gtk_container_add(GTK_CONTAINER(align), label);
			gtk_widget_show(label);
		}
	}
}

/////////////////////////////// TODO /////////////////////////////////////////////////////////////
/////////////////////////////// TODO /////////////////////////////////////////////////////////////

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
	printf("%s\n", (char *) g_client);

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

	// Signal to kill main gtk loop
	g_signal_connect(chat_window, "destroy", G_CALLBACK(destroy_chat_window), client);
}
