/*
 * MainWindow.cpp
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
#include "GTKFunctions.h"
#include "../Client.h"

using namespace std;

// Client info
extern Client * current_client;

// Map used for correspondence between client - chat window
extern map <string, GtkWidget *> map_chat_windows;

/**
 * TODO
 */
static void execute_menu_item(GtkWidget * widget, gpointer g_client) {
	printf("test\n");
}

enum one_entry_type {NORMAL_ENTRY, DELETE_GROUP, CHANGE_AVAILABILITY};

/**
 * Create a window with only one entry
 */
static void create_one_entry_window(gint width, gint height, gchar * title, gchar * label_text,
		gint chars_limit, gchar * button_text, void (* handler)(struct _general_info *),
		one_entry_type type) {
	// Create new window
	GtkWidget * window = create_new_window(width, height, title);

	// Create vbox and it's alignment
	GtkWidget * new_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	GtkWidget * new_vbox = create_aligned_vbox(window, new_vbox_align);

	// Create field / combo zone
	create_label_field(new_vbox, string(label_text));
	map <string, vector <User *> > groups;
	GtkWidget * entry;
	switch (type) {
		case DELETE_GROUP:
			entry = gtk_combo_box_text_new();
			groups = current_client->get_groups();
			for (map <string, vector <User *> >::iterator it = groups.begin(); it != groups.end();
					it++) {
				gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry), it->first.c_str());
			}
			break;
		case CHANGE_AVAILABILITY:
			entry = gtk_combo_box_text_new();
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry), "available");
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry), "away");
			gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(entry), "busy");
			break;
		default:
			entry = gtk_entry_new();
			gtk_entry_set_max_length(GTK_ENTRY(entry), chars_limit);
			break;
	}
	add_vbox_row(new_vbox, entry, width / 1.5, 0);

	// Create recovery button
	GtkWidget * button = gtk_button_new_with_label(button_text);
	add_vbox_row(new_vbox, button, 0, 0);

	// Action on button
	struct _general_info * ng_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	ng_info->window_top_level = window;
	ng_info->entry = entry;
	g_signal_connect_swapped(button, "clicked", (GCallback) handler, (gpointer) ng_info);

	gtk_widget_show_all(window);
}

/**
 * Add a new friend
 */
static void add_friend_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Add friend",
			(gchar *) "Enter friend's username:", MAX_REGISTER_CHARS, (gchar *) "Add friend",
			signal_add_friend, NORMAL_ENTRY);
}

/**
 * Create a new group
 */
static void create_group_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Create group",
			(gchar *) "Enter group name:", MAX_REGISTER_CHARS, (gchar *) "Create group",
			signal_create_group, NORMAL_ENTRY);
}

/**
 * Delete an existing group
 */
static void delete_group_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Delete group",
			(gchar *) "Choose group name:", MAX_REGISTER_CHARS, (gchar *) "Delete group",
			signal_delete_group, DELETE_GROUP);
}

/**
 * Change status
 */
static void change_status_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Change status",
			(gchar *) "Enter new status:", MAX_STATUS_CHARS, (gchar *) "Change status",
			signal_change_status, NORMAL_ENTRY);
}

/**
 * Change availability
 */
static void change_availability_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Change availability",
			(gchar *) "Choose new availability:", MAX_REGISTER_CHARS, (gchar *) "Change availability",
			signal_delete_group, CHANGE_AVAILABILITY);
}

/**
 * Create a entry for the context menu
 */
inline static void create_menu_entry(GtkWidget * menu, gchar * label_text,
		void (* handler)(GtkWidget *, gpointer), gpointer data) {
	GtkWidget * menu_item = gtk_menu_item_new_with_label(label_text);
	g_signal_connect(menu_item, "activate", (GCallback) handler, data);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	gtk_widget_show(menu_item);
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

		// Create context menu entries
		create_menu_entry(context_menu, (gchar *) "Start chat", clientgtk_create_chat_window, g_client);
		create_menu_entry(context_menu, (gchar *) "Send file", signal_send_file, g_client);
		create_menu_entry(context_menu, (gchar *) "Show profile", signal_show_profile, g_client);
		create_menu_entry(context_menu, (gchar *) "Move to group", signal_move_to_group, g_client);

		// Create menu
		gtk_menu_popup(GTK_MENU(context_menu), NULL, NULL, NULL, NULL,
				(event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent *) event));
	}
}

/**
 * Create main interface which contains friends // TODO change strings into defines and functions
 */
void clientgtk_create_main_window(GtkWidget * window_top_level) {
	// Create a vbox that contains the menu bar and the scrolled window
	GtkWidget * vbox = gtk_vbox_new(FALSE, 0);
	gtk_container_add(GTK_CONTAINER(window_top_level), vbox);
	gtk_widget_show(vbox);

	// Create menu bar
	GtkWidget * menu_bar = gtk_menu_bar_new();
	gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
	gtk_widget_show(menu_bar);

	GtkWidget * submenu1 = create_menu_bar_submenu(menu_bar, (gchar *) "Friends");
	create_menu_entry(submenu1, (gchar *) "Add friend", add_friend_window, NULL);
	create_menu_entry(submenu1, (gchar *) "Search friends", execute_menu_item, NULL);
	create_menu_entry(submenu1, (gchar *) "Create group", create_group_window, NULL);
	create_menu_entry(submenu1, (gchar *) "Delete group", delete_group_window, NULL);

	GtkWidget * submenu2 = create_menu_bar_submenu(menu_bar, (gchar *) "Account");
	create_menu_entry(submenu2, (gchar *) "Change status", change_status_window, NULL);
	create_menu_entry(submenu2, (gchar *) "Change availability", change_availability_window, NULL);
	create_menu_entry(submenu2, (gchar *) "Show my profile", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Update profile", execute_menu_item, NULL);

	GtkWidget * submenu3 = create_menu_bar_submenu(menu_bar, (gchar *) "Settings");
	struct _general_info * g_info = (struct _general_info *) malloc(sizeof(struct _general_info));
	g_info->window_top_level = window_top_level;
	g_info->vbox_align = vbox;
	create_menu_entry(submenu3, (gchar *) "Logout", signal_logout, (gpointer) g_info);

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

	// Show groups and friends
	GtkWidget * label, * align;
	map <string, vector <User *> > groups = current_client->get_groups();
	for (map <string, vector <User *> >::iterator it = groups.begin(); it != groups.end(); it++) {
		// Doesn't show group if it has no users
		if (it->second.size() == 0) {
			continue;
		}
		// Show group
		align = gtk_alignment_new(0.5, 0.5, 0, 0);
		gtk_box_pack_start(GTK_BOX(main_vbox), align, FALSE, FALSE, 0);
		gtk_widget_show(align);
		label = gtk_label_new("");
		gtk_label_set_markup(GTK_LABEL(label),
				("<big><b><u>" + it->first + "</u></b></big>").c_str());
		gtk_container_add(GTK_CONTAINER(align), label);
		gtk_widget_show(label);

		// Show users
		for (vector <User *>::iterator user = it->second.begin(); user != it->second.end(); user++) {
			// Create hbox
			GtkWidget * hbox = gtk_hbox_new(FALSE, 0);
			gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 0);

			// Create friend button
			string client = (*user)->get_username();
			add_button_to_box(hbox, client, TRUE, check_friend_clicks, TRUE);

			// Create state label TODO
			string state = (*user)->get_state_as_string();
			align = gtk_alignment_new(0, 0.5, 0, 0);
			gtk_box_pack_start(GTK_BOX(hbox), align, FALSE, FALSE, 0);
			label = gtk_label_new("");
			gtk_label_set_markup(GTK_LABEL(label), ("<big>" + state + "</big>").c_str());
			gtk_container_add(GTK_CONTAINER(align), label);

			// Show all widgets
			gtk_widget_show_all(hbox);
		}
	}
}
