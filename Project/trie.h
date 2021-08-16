#ifndef TRIE_H_INCLUDED
#define TRIE_H_INCLUDED

#define ALPHABET_SIZE 26
#define CHAR_INDEX(c) ((int)c - (int)'a') //(c-'a')

#include "gui.h"

typedef struct trie{
    int isEOW;
    struct trie* children[ALPHABET_SIZE];
}trie;

typedef trie* trieptr;

trieptr create_trie_node();
void create_trie(trieptr* t);
void init_trie(trieptr* t);
void insert_trie(trieptr* t, const char* word);
int search_trie(trieptr t, char* word);
int isLast(trieptr t);
int autocomplete(trieptr t, char* str, FILE* fd);
void suggest(trieptr t, char* str, FILE* fd);

char* append(char* string, char c);
char* remove_last(char* string);

void free_trie(trieptr* t);
#endif // TRIE_H_INCLUDED
