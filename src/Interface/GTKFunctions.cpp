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
