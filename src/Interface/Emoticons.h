/*
 * Emoticons.h
 *
 *  Created on: May 15, 2012
 *      Author: mihailc
 */

#ifndef EMOTICONS_H_
#define EMOTICONS_H_

#define EMOTICONS_FOLDER	"Emoticons/"
#define EMOTICONS_FILE		"emoticons.txt"
#define MAX_EMOTICONS		100

/**
 * Create emoticons map
 */
std::map <std::string, std::string> emoticons_create_map(std::string filename);

/**
 * Split a text based on emoticons
 */
std::vector <std::string> emoticons_split_text(std::string text,
		std::map <std::string, std::string> emoticons);

/**
 * Print a emoticon to a text view
 */
void emoticons_print(GtkWidget * conversation_chat, std::string emoticon_code,
		std::map <std::string, std::string> emoticons);

/**
 * Print a vector of strings to a text view
 */
void emoticons_print_text(GtkWidget * conversation_chat, std::vector <std::string> strs,
		std::map <std::string, std::string> emoticons);

#endif /* EMOTICONS_H_ */
