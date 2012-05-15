/*
 * Emoticons.cpp
 *
 *  Created on: May 15, 2012
 *      Author: mihailc
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <cassert>
#include <cstring>
#include <regex.h>

#include <gtk/gtk.h>

#include "Emoticons.h"
#include "ClientGTK.h"
#include "../ServerFunctions.h"

using namespace std;

/**
 * Create emoticons map
 */
map <string, string> emoticons_create_map(std::string filename) {
	map <string, string> emoticons;

	ifstream ifs(filename.c_str(), ifstream::in);
	assert(!ifs.fail());

	string line;
	while (!ifs.eof()) {
		getline(ifs, line);
		vector<string> tokens;
		tokenize(line, tokens, " ");

		if (tokens.size() == 2) {
			emoticons.insert(pair<string, string> (tokens[0], tokens[1]));
		}
	}

	return emoticons;
}

/**
 * Split a text based on emoticons
 */
vector <string> emoticons_split_text(string text, map <string, string> emoticons) {
	// Create patern
	map <string, string>::iterator it = emoticons.begin();
	string patern = "(";
	bool first = true;
	for (; it != emoticons.end(); it++) {
		if (first) {
			first = false;
		} else {
			patern += "|";
		}
		patern += it->first;
	}
	patern += ")";

	// Split text
	vector <string> strs;
	regex_t preg;
	regcomp(&preg, patern.c_str(), REG_EXTENDED);
	regmatch_t pmatch[1];
	string aux = text;
	while(1) {
		if (regexec(&preg, aux.c_str(), 1, pmatch, 0)) {
			break;
		}
		cout << aux.substr(0, pmatch[0].rm_so) << endl;
		cout << aux.substr(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so) << endl;

		strs.push_back(aux.substr(0, pmatch[0].rm_so));
		strs.push_back(aux.substr(pmatch[0].rm_so, pmatch[0].rm_eo - pmatch[0].rm_so));
		aux = aux.substr(pmatch[0].rm_eo);
	}
	if (aux.length() > 0) {
		strs.push_back(aux);
	}
	cout << aux << endl;
	return strs;
}


/**
 * Print a emoticon to a text view
 */
void emoticons_print(GtkWidget * conversation_chat, string emoticon_code,
		map <string, string> emoticons) {
	// Find emoticon filename
	string emoticon_filename = emoticons.find(emoticon_code)->second;
	string filename = string(EMOTICONS_FOLDER) + emoticon_filename;

	// Print emoticon
	GError * error = NULL;
	GdkPixbuf * pixbuf = gdk_pixbuf_new_from_file(filename.c_str(), &error);
	if (error) {
		clientgtk_create_message_dialog("Could not retrieve emoticon", "Error emoticons",
				GTK_MESSAGE_ERROR);
		g_error_free(error);
	} else {
		GtkTextIter iter;
		GtkTextBuffer * buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(conversation_chat));
		GtkTextMark * cursor = gtk_text_buffer_get_insert(buffer);
		gtk_text_buffer_get_iter_at_mark(buffer, &iter, cursor);
		gtk_text_buffer_insert_pixbuf(buffer, &iter, pixbuf);
	}
}

/**
 * Print a vector of strings to a text view
 */
void emoticons_print_text(GtkWidget * conversation_chat, vector <string> strs,
		map <string, string> emoticons) {
	GtkTextMark * cursor;
	GtkTextIter iter;
	GtkTextBuffer * buffer;

	int n = 0;
	for (vector <string>::iterator it = strs.begin(); it != strs.end(); it++) {
		if (n % 2 == 1) {
			stringstream emoticon_code;
			for (string::iterator c = (*it).begin(); c != (*it).end(); c++) {
				emoticon_code << "\\" << *c;
			}

			emoticons_print(conversation_chat, emoticon_code.str(), emoticons);
		} else {
			buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(conversation_chat));
			cursor = gtk_text_buffer_get_insert(buffer);
			gtk_text_buffer_get_iter_at_mark(buffer, &iter, cursor);
			gtk_text_buffer_insert(buffer, &iter, (*it).c_str(), strlen((*it).c_str()));
		}
		n++;
	}

	// Show text in visible area
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(conversation_chat));
	cursor = gtk_text_buffer_get_insert(buffer);
	gtk_text_buffer_get_iter_at_mark(buffer, &iter, cursor);
	gtk_text_view_scroll_to_iter(GTK_TEXT_VIEW(conversation_chat), &iter, 0, FALSE, 0, 0);
}

