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

int main(int argc, const char *argv[]) {
	int  h, w, fd, i = 0, c;
	char ch, *filename;
	buffptr bf = NULL;


	init_curses();
	getmaxyx(stdscr, h, w);
	h = h-1;
	init_buffer(&bf, h*w);
	attron(COLOR_PAIR(2));

	help();
	wbkgd(stdscr, COLOR_PAIR(2));

	if(argc == 2){

		strcpy(filename, argv[1]);
		if(fileexist(argv[1])){
			fd = open(argv[1], O_RDWR);
			fill_buffer(fd, &bf, h, w);
			fill_screen(bf, h, w);
			info(filename);
			close(fd);
		}

	}
	else if(argc == 1){
		strcpy(filename, "UNTITLED_DOCUMENT");
	}
	else{
		attron(A_BOLD);
		attron(A_BLINK);
		attron(COLOR_PAIR(4));
		mvprintw(h/2-5, w/2-5, "Usage: %s <filename> ", argv[0]);
		attroff(A_BOLD);
		attroff(A_BLINK);
		mvprintw(h/2-4, w/2-5, "Press any key to exit");
		attroff(COLOR_PAIR(4));
		getch();
		endwin();
		exit(1);
	}

	input_handle(&bf,filename);
	attroff(COLOR_PAIR(2));
	//getch();
	endwin();
	free_buffer(&bf);
	return 0;
}
