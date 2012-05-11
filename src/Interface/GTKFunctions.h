/*
 * GTKFunctions.h
 *
 *  Created on: May 11, 2012
 *      Author: mihailc
 */

#ifndef GTKFUNCTIONS_H_
#define GTKFUNCTIONS_H_

/**
 * Create a new window
 */
GtkWidget * create_new_window(gint width, gint height, gchar * title);

/**
 * Create an aligned vbox
 */
GtkWidget * create_aligned_vbox(GtkWidget * window_top_level, GtkWidget * vbox_align);

/**
 * Add a entry/button to a vbox and center it
 */
void add_vbox_row(GtkWidget * vbox, GtkWidget * entry, gint width, gint height);

/**
 * Create a label field
 */
void create_label_field(GtkWidget * vbox, std::string label_text);

#endif /* GTKFUNCTIONS_H_ */