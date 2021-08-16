#include <ncurses.h>
#include "gap.h"

void init_curses();
int fileexist(const char* filename);
void fill_screen(buffptr bf, int h, int w);
void input_handle(buffptr* b, char* filename);
void info(char* filename);
void help();
