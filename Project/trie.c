#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "gap.h"
#include "trie.h"
#include "gui.h"

void init_trie(trieptr* t){
    *t = NULL;
}

trieptr create_trie_node(){
    trieptr nn = NULL;
    nn = (trieptr)malloc(sizeof(trie));

    if(nn){
        nn->isEOW = 0;
        int i;
        for(i = 0; i < ALPHABET_SIZE; i++){
            nn->children[i] = NULL;
        }
    }
    return nn;
}

void insert_trie(trieptr *t, const char* word){

    int length = strlen(word);
    int i = 0;
    int index;
    trieptr p = *t;

    if(!p){
        *t = create_trie_node();
    }
    p = *t;

    for(i = 0; i < length; i++){
        index = CHAR_INDEX(word[i]);
        if(!p->children[index]){
            p->children[index] = create_trie_node();
        }
        p = p->children[index];
    }
    p->isEOW++;
}

int search_trie(trieptr t, char* word){
    int length = strlen(word);
    int i = 0;
    int index;
    if(!t)
        return 0;

    for(i = 0; i < length; i++){
        index = CHAR_INDEX(word[i]);
        if(!t->children[index])
            return 0;
        t = t->children[index];
    }
    if(t && t->isEOW != 0)
        return 1;
    else
        return 0;
}

int isLast(trieptr t){
    int i = 0;
    for(i = 0; i < ALPHABET_SIZE; i++){
        if(t->children[i])
            return 0;
    }
    return 1;
}
int autocomplete(trieptr t, char* str,FILE* fd){
    trieptr p = t;

    int length = strlen(str);
    int i;
    int index;
    for(i = 0; i < length; i++){
        index = CHAR_INDEX(str[i]);
        if(!p->children[index])
            return 0;
        p = p->children[index];
    }
    int isWord = p->isEOW;
    int last = isLast(p);

    if(isWord && last){
        return -1;
    }
    if(!last){
        suggest(p, str, fd);
        return 1;
    }
    return 0;
}

void suggest(trieptr t, char* str, FILE* fd){
    if(t->isEOW){
    	fprintf(fd, "%s\n", str);
    }
    if(isLast(t))
        return;

    for(int i = 0; i < ALPHABET_SIZE; i++){
        if(t->children[i]){
            char c = i + 97;
            str = append(str, c);
            suggest(t->children[i], str,fd);
            str = remove_last(str);
        }
    }
    return;
}

char* append(char* string, char c){
    int length = strlen(string);

    string = (char*)realloc(string, sizeof(char)*(length + 1));
    string[length] = c;
    string[length + 1] = '\0';

    return string;

}
char* remove_last(char* string){
    int length = strlen(string);
    string = (char*)realloc(string, sizeof(char)*(length - 1));
    string[length - 1] = '\0';
    return string;
}

void create_trie(trieptr* t){
	char str[100];
	init_trie(t);
	FILE* f = fopen("auto.txt", "r+");
	if(f){
		while(fscanf(f, "%s", str) != EOF){
			insert_trie(t, str);
		}
	}
	fclose(f);
	//mvprintw(0,1,"%d ", search_trie(*t, "pharmaceutical"));
	//getch();
}

void free_trie(trieptr* t){
	trieptr tt = *t;
	int i;
	if(!tt)
		return;
		
	for(i = 0; i < 26; i++){
		free_trie(&tt->children[i]);
	}
	free(*t);
	*t = NULL;
}
