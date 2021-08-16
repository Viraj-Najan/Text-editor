#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "gap.h"
#include "gui.h"


void init_buffer(buffptr* bf, int size){
	buffptr b = *bf;
	if(b == NULL){
		*bf = (buffptr)malloc(sizeof(buffer));
	}
	b = *bf;
    b->buff = (char*)malloc(sizeof(char)*size);
    b->gapsize = size;
    b->size = size;
    b->gapleft = 0;
    b->gapright = b->gapsize - b->gapleft - 1;
    memset(b->buff, '.', b->gapsize);
    b->next = NULL; 
    b->prev = NULL; 
}

void left(buffptr b, int position){
    while(position < b->gapleft){

        b->gapleft--;
        b->gapright--;
        b->buff[b->gapright+1] =b->buff[b->gapleft];
        b->buff[b->gapleft] = '.';
    }
}

void right(buffptr b, int position){
    while(position > b->gapleft){
        b->gapleft++;
        b->gapright++;
        b->buff[b->gapleft-1] =b->buff[b->gapright];
        b->buff[b->gapright] = '.';
    }
}

void insert_buffer(char str, int position, buffptr b){
    if(position < 0 || position >= b->size)
        return;

    if(b->gapleft != position)
        move_cursor(b, position);

    if(b->gapleft == b->gapright){
        grow(b, position, 10);
    }
    b->buff[b->gapleft] = str;
    b->gapleft++;
    b->gapsize--;

    return;
}

void delete_buffer(int position, buffptr b){
	if(position+1 != b->gapleft)
		move_cursor(b, position+1);
		
	b->gapleft--;
	b->gapsize++;
	b->buff[b->gapleft] = '.';
}

void move_cursor(buffptr b, int position){
    if(b->gapleft > position)
        left(b, position);
    else if(b->gapleft < position)
        right(b, position);
}

void grow(buffptr b, int position, int gap){
    char a[b->size];
    int i;

    for(i = position + 1; i < b->size; i++){
        a[i - (position + 1)] = b->buff[i];
    }
    int j = i - (position + 1);

    int new_size = b->size + gap - 1;
    b->size = new_size;
    b->buff = (char*)realloc(b->buff, sizeof(char)*b->size);

    for(i = 0; i < gap; i++){
        b->buff[i + position] = '.';
    }
    for(i = 0; i < j; i++){
        b->buff[i+position+gap] = a[i];
    }
    b->gapsize = gap;
    b->gapleft = position;
    b->gapright = position + gap - 1;
}

void fill_buffer(int fd, buffptr* bf, int h, int w){
	char ch;
	int err, i = 0, l = 0;
	
	buffptr b = *bf;

	while((err = read(fd, &ch, 1))){
		if(err == -1){
			perror("Read Error: ");
			exit(0);
		}
		if(ch == '\n'){
			l++;
		}
		if(l >= h || i >= (h*w) - 1){
			init_buffer(&b->next, h*w);
			b->next->prev = b;
			b = b->next;
			i = 0;
			l = 0;
		}
		insert_buffer(ch, i, b);
		i++;

	}	
}
/*
int is_fill(buffptr b, int i){
	if(i < b->size){
		if(i >= b->gapleft && i <= b->gapright)
			return 0;
		else
			return 1;
	}
	else
		return 0;
}
*/
void load_file(buffptr b, int fd){
	if(lseek(fd, 0, SEEK_SET) == -1){
		perror("lseek error: ");
	}
	while(b){
		if((write(fd, b->buff, b->gapleft)) == -1){
			perror("Write Error: ");
		};
		if((write(fd, b->buff+b->gapright+1, b->size-b->gapright-1)) == -1){
			perror("Write Error: ");
		};
		b = b->next;
	}
}

void free_buffer(buffptr* b){
	buffptr bf = *b;
	if(!bf)
		return;
	
	free_buffer(&bf->next);
	
	free(bf->buff);
	bf->buff = NULL;
	bf->prev = NULL;
	free(*b);
	*b = NULL;
}
