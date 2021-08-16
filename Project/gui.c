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

#define CTRL_KEYPRESS(s) ((s) & 0x1f)

void init_curses(){
	initscr();
	noecho();
	//cbreak();
	raw();
	keypad(stdscr, true);
	curs_set(1);
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_WHITE, COLOR_BLACK);
	init_pair(4, COLOR_RED, COLOR_BLUE);
	init_pair(5, COLOR_WHITE, COLOR_CYAN);
	
	clear();
	refresh();
}

int fileexist(const char* filename){
	int fd = open(filename, O_RDWR);
	
	if(fd == -1)
		return 0;
	close(fd);
	return 1;	
}

void fill_screen(buffptr bf, int h, int w){
	int x = 0;
	int y = 0;
	int i = 0;
	char ch;
	
	if(bf == NULL)
		return;
	clear();
		
	for(x = 0; x < h; x++){
		for(y = 0; y < w; y++){
			if(i == bf->gapleft){
				while(i <= bf->gapright)
					i++;
			}
			if(i >= bf->size)
				break;
			ch = bf->buff[i];
			i++;
			if(ch == '\n'){
				mvprintw(x,y,"%c",ch);
				break;
			}
			else if(ch == '\t'){
				mvprintw(x,y,"    ");
				y+=4;

			}
			else
				mvprintw(x,y,"%c",ch);
			
		}	
		if(i >= bf->size)
			break;	
	}
}
void input_handle(buffptr* b,char* filename){
	int h, w, x, y, i,ch, fd, saveflag;
	buffptr bf = *b;
	buffptr temp = *b;

	
	x=y=0;
	getmaxyx(stdscr,h,w);
	h = h-1;
	getyx(stdscr,x,y);
	info(filename);
	trieptr t;
	create_trie(&t);

	while(ch = getch()){
		switch(ch){
			case KEY_UP:
				getyx(stdscr,x,y);
				if(x>0){
					move(--x, y);	
					info(filename);
					move(x, y);
				}
				else if(x == 0 && bf->prev != NULL){
					bf = bf->prev;
					fill_screen(bf, h, w);
					x = h-1;
					move(x, y);
					info(filename);
					move(x, y);
				}
				break;
			
			case KEY_DOWN:
				getyx(stdscr,x,y);
				if(x < h-1){
					move(++x, y);
					info(filename);
					move(x, y);	
				}
				else if(x == h-1 && bf->next != NULL){
					bf = bf->next;
					fill_screen(bf, h, w);
					x = 0;
					move(x, y);
					info(filename);
					move(x, y);
				}
				break;
			
			case KEY_LEFT:
				getyx(stdscr,x,y);
				if(y > 0){
					move(x, --y);	
					info(filename);
					move(x, y);
				}
				else if(x == 0 && y == 0 && bf->prev != NULL){
					bf = bf->prev;
					fill_screen(bf, h, w);
					info(filename);
					move(x, y);
				}
				else if(y == 0){
					y = w-1;
					move(--x, y);
					info(filename);
					move(x, y);
				}
				break;
				
			case KEY_RIGHT:
				getyx(stdscr,x,y);
				if(y < w-1){
					move(x, ++y);
					info(filename);
					move(x, y);
				}
				else if(x == h-1 && y == w-1 && bf->next != NULL){
					bf = bf->next;
					fill_screen(bf, h, w);
					x = 0;
					y = 0;
					move(x, y);
					info(filename);
					move(x, y);
				}
				else if(y == w-1){
					x++;
					y = 0;
					move(x, y);
					info(filename);
					move(x, y);
				}
				break;
				
				
			case KEY_NPAGE:
				fill_screen(bf->next, h, w);
				getyx(stdscr,x,y);
				info(filename);
				move(x, y);
				if(bf->next){
					bf = bf->next;
				}
				break;
			
			case KEY_PPAGE:
				fill_screen(bf->prev, h, w);
				getyx(stdscr,x,y);
				info(filename);
				move(x, y);
				if(bf->prev){
					bf = bf->prev;
				}
				break;
				
			case KEY_BACKSPACE:
			case KEY_DC:
				getyx(stdscr,x, y);
				if(x == 0 && y== 0){
					if(!bf->prev)
						continue;
					else{
						bf = bf->prev;
						x = h-1;
						y = w-1;
						i = x*w + y;
						delete_buffer(i, bf);							
					}
				}
				if(y == 0){
					x--;
					y = w;
				}
				y--;
				i = x*w + y;
				delete_buffer(i, bf);
				fill_screen(bf, h, w);
				move(x,y);
				info(filename);
				move(x, y);		

				break;
			case 19://ctrl+s;
				fd = open(filename, O_RDWR| O_CREAT |O_TRUNC, S_IRWXU);
				load_file(temp, fd);
				close(fd);	
				
				saveflag = 1;
				break;

			case KEY_END:
				if(saveflag == 1){
					free_trie(&t);
					free_buffer(&temp);
					temp = NULL;
					*b = NULL;
					return;
				}
				else{
					WINDOW* win = newwin(6, 50, h/2 - 5, w/2 - 10);
					refresh();
					box(win, 0, 0);
					mvwprintw(win, 1, 1, "Do You want to close without saving");
					mvwprintw(win, 3, 1, "YES -> Press end");
					mvwprintw(win, 4, 1, "NO  -> Press Enter");
					wrefresh(win);
					int c1;
					c1 = getch();
					if(c1 == 10){
						fd = open(filename, O_RDWR| O_CREAT |O_TRUNC, S_IRWXU);
						load_file(temp, fd);
						close(fd);
						saveflag = 1;		
					}
					else if(c1 == KEY_END){
						free_trie(&t);
						free_buffer(&temp);
						temp = NULL;
						*b = NULL;
						return;
					}
					wclear(win);
					wrefresh(win);
					delwin(win);
					refresh();
				}
				break;
				
			case 9://tab
				refresh();
				getyx(stdscr,x, y);
				int j = x*w + y, k = 0, l, a = 0, x1, y1;
				FILE* f2;
				char str[100];
				while(j > 0 && bf->buff[j-1] != 32 && bf->buff[j-1] != 10){
					j--;
					k++;
				}
				char *word = (char*)malloc(sizeof(char)*(k+1));
				for(l = 0; l < k; l++){
					word[l] = bf->buff[j];
					j++;
				}
				word[l] = '\0';
				f2 = fopen("suggetions.txt","w+");

				WINDOW* win2 = newwin(50, 200, 0, 0);
				box(win2, 0, 0);
				wrefresh(win2);
				if(word[0] != '\0')
					a = autocomplete(t, word, f2);
				fclose(f2);	
				
				f2 = fopen("suggetions.txt","r");
				if(a == -1){
					wclear(win2);
					box(win2, 0, 0);
					mvwprintw(win2, 1, 1, "Thats the only word");	
					wrefresh(win2);
				}
				else if(a == 0){
					wclear(win2);
					box(win2, 0, 0);
					mvwprintw(win2, 1, 1, "NO Word Found");	
					wrefresh(win2);
				}
				else{
					//getparyx(win2, x1, y1);
					x1 = 1;
					y1 = 2;
					while(fgets(str, 100, f2) != NULL){
						if(x1 == 47){
							x1 = 1;
							y1 = y1+20;
						}
						mvwprintw(win2, x1, y1, "%s ", str);
						x1++;
						wrefresh(win2);

					}
					box(win2, 0, 0);
					wrefresh(win2);
				}
				mvwprintw(win2, 48, 1,"Press Any key to continue....");
				wrefresh(win2);
				getch();
				wclear(win2);
				wrefresh(win2);
				delwin(win2);
				fill_screen(bf, h, w);
				info(filename);
				move(x, y);
				fclose(f2);	
				free(word);
				break;
				
			case 10: //Enter
				saveflag = 0;
				getyx(stdscr, x, y);
				if(y < w-1 && x < h-1){
					i = x*w + y;
					while(y < w-1){
						insert_buffer(' ', i, bf);	
						i++;
						y++;
					}
					i = x*w + y;
					insert_buffer(ch, i, bf);	
					y++;
				}
				else if(x == h-1 && y <= w-1){
					i = x*w + y;
					while(y < w-1){
						insert_buffer(' ', i, bf);	
						i++;
						y++;
					}
					insert_buffer(ch, i, bf);	
					y++;
					if(bf->next == NULL)
						init_buffer(&bf->next, h*w);
					bf->next->prev = bf;
					bf = bf->next;
					x = 0;
					y = 0;
					
				}
				else{
					i = x*w + y;
					insert_buffer('\n', i, bf);	
				}
				fill_screen(bf, h, w);
				getyx(stdscr, x, y);
				move(x,y);
				info(filename);
				move(x, y);
				break;
			
			case KEY_HOME:
				help();
				fill_screen(bf, h, w);
				break;
			
			default:
				saveflag = 0;
				getyx(stdscr, x, y);
				i = x*w + y;
				if(x == h-1 && y == w-1){
					if(bf->next == NULL)
						init_buffer(&bf->next, h*w);
					bf->next->prev = bf;
					bf = bf->next;
					x = 0;
					y = 0;
				}
				i = x*w + y;
				insert_buffer(ch, i, bf);
				fill_screen(bf, h, w);
				if(y == w-1){
					x++;
					y = -1;
				}
				y++;
				move(x,y);
				info(filename);
				move(x, y);
				break;
			                                                                                                                                                                                            
			refresh();
				
		}	
	}
}

void info(char* filename){
	int h, w, x, y;
	getmaxyx(stdscr, h, w);
	getyx(stdscr, x, y);
	attron(A_BOLD);
	attron(COLOR_PAIR(3));
	mvprintw(h-1, w/2, "Filename: %s | Line: %d, Col: %d", filename, x, y);
	move(0, 0);
	attroff(A_BOLD);
	attroff(COLOR_PAIR(3));
}

void help(){
	clear();
	wbkgd(stdscr, COLOR_PAIR(1));
	int h, w;
	getmaxyx(stdscr, h, w);
	attron(COLOR_PAIR(1));
	mvprintw(h/2-5, w/2-5, "SAVE      : ctrl+s");
	mvprintw(h/2-4, w/2-5, "HELP      : home");
	mvprintw(h/2-3, w/2-5, "QUIT      : end");
	mvprintw(h/2-2, w/2-5, "MOVE      : arrow key");
	mvprintw(h/2-1, w/2-5, "NEXT PAGE : Page dn");
	mvprintw(h/2, w/2-5, "PREV PAGE : Page up");
	mvprintw(h/2+1, w/2-5, "Suggetions: Tab");
	mvprintw(h/2+2, w/2-5, "Press any key to Continue");
	attroff(COLOR_PAIR(1));	
	getch();
	clear();
	wbkgd(stdscr, COLOR_PAIR(2));
	refresh();
}

