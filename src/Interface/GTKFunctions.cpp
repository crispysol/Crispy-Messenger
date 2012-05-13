/*
 * GTKFunctions.cpp
 *
 *  Created on: May 11, 2012
 *      Author: mihailc
 */

#include <string>

#include <gtk/gtk.h>

using namespace std;

/**
 * Create a new window
 */
GtkWidget * create_new_window(gint width, gint height, gchar * title) {
	GtkWidget * window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), title);
	gtk_window_set_default_size(GTK_WINDOW(window), width, height);

	g_signal_connect(window, "destroy", G_CALLBACK(gtk_widget_destroy), NULL);

	return window;
}

/**
 * Create an aligned vbox
 */
GtkWidget * create_aligned_vbox(GtkWidget * window_top_level, GtkWidget * vbox_align) {
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
void add_vbox_row(GtkWidget * vbox, GtkWidget * entry, gint width, gint height) {
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
void create_label_field(GtkWidget * vbox, string label_text) {
	GtkWidget * label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label), ("<big>" + label_text + "</big>").c_str());
	gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 0);
	gtk_widget_show(label);
}

/**
 * Create a button and add it to a box
 */
void add_button_to_box(GtkWidget * box, string label_text, gboolean aligned,
		void (* handler)(GtkWidget *, GdkEventButton *, gpointer), gpointer g_client) {
	GtkWidget * align, * button, * label;
	// Create button
	button = gtk_button_new();
	gtk_button_get_focus_on_click(GTK_BUTTON(button));
	gtk_button_set_relief(GTK_BUTTON(button), GTK_RELIEF_NONE);
	gtk_box_pack_start(GTK_BOX(box), button, aligned, aligned, 0);

	// Action on click
	g_signal_connect(button, "button_press_event", G_CALLBACK(handler), (gpointer) g_client);

	// Button label and it's alignment
	align = gtk_alignment_new(0, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER(button), align);
	label = gtk_label_new("");
	gtk_label_set_markup(GTK_LABEL(label), ("<big>" + label_text + "</big>").c_str());
	gtk_container_add(GTK_CONTAINER(align), label);
}

/**
 * Create chat text in a scrolled window
 */
GtkWidget * scrolled_chat_text_view(GtkWidget * vbox, gboolean from_start) {
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
