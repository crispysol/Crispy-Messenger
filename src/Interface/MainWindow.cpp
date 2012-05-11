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

using namespace std;

// Map used for correspondence between client - chat window
extern map <string, GtkWidget *> map_chat_windows;

/**
 * TODO
 */
static void execute_menu_item(GtkWidget * widget, gpointer g_client) {
	printf("test\n");
}

/**
 * Create a window with only one entry
 */
static void create_one_entry_window(gint width, gint height, gchar * title, gchar * label_text,
		gchar * button_text, void (* handler)(struct _general_info *)) {
	// Create new window
	GtkWidget * window = create_new_window(width, height, title);

	// Create vbox and it's alignment
	GtkWidget * new_vbox_align = gtk_alignment_new(0.5, 0.4, 0, 0);
	GtkWidget * new_vbox = create_aligned_vbox(window, new_vbox_align);

	// Create email field TODO limit nr chars
	create_label_field(new_vbox, string(label_text));
	GtkWidget * entry = gtk_entry_new();
	add_vbox_row(new_vbox, entry, width / 2, 0); // TODO define

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
			(gchar *) "Enter friend's username:", (gchar *) "Add friend", signal_add_friend);
}

/**
 * Create a new group
 */
static void create_group_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Create group",
			(gchar *) "Enter group name:", (gchar *) "Create group", signal_create_group);
}

/**
 * Delete an existing group
 */
static void delete_group_window(GtkWidget * widget, gpointer info = NULL) {
	create_one_entry_window(AUX_WINDOW_WIDTH, AUX_WINDOW_HEIGHT, (gchar *) "Delete group",
			(gchar *) "Enter group name:", (gchar *) "Delete group", signal_delete_group);
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

		// Create context menu entries // TODO change strings into defines and functions
		create_menu_entry(context_menu, (gchar *) "Start chat",
				clientgtk_create_chat_window, g_client);
		create_menu_entry(context_menu, (gchar *) "Send file",
				signal_send_file, g_client);
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
 * Create main interface which contains friends // TODO change strings into defines and functions
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
	create_menu_entry(submenu1, (gchar *) "Add friend", add_friend_window, NULL);
	create_menu_entry(submenu1, (gchar *) "Search friends", execute_menu_item, NULL);
	create_menu_entry(submenu1, (gchar *) "Create group", create_group_window, NULL);
	create_menu_entry(submenu1, (gchar *) "Delete group", delete_group_window, NULL);

	GtkWidget * submenu2 = create_menu_bar_submenu(menu_bar, (gchar *) "Account");
	create_menu_entry(submenu2, (gchar *) "Change status", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Change availability", execute_menu_item, NULL);
	create_menu_entry(submenu2, (gchar *) "Show my profile", execute_menu_item, NULL);
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
