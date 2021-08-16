#ifndef GAP_H_INCLUDED
#define GAP_H_INCLUDED

#define SIZE 10000

typedef struct buffer{
    char *buff;
    int size;
    int gapsize;
    int gapleft;
    int gapright;
    struct buffer* next;
    struct buffer* prev;
}buffer;

typedef buffer* buffptr;

void init_buffer(buffptr* b, int size);
void left(buffptr b, int position);
void right(buffptr b, int position);
void insert_buffer(char str, int position, buffptr b);
void move_cursor(buffptr b, int position);
void grow(buffptr b, int position, int end);
void fill_buffer(int fd, buffptr* b, int h, int w);
void delete_buffer(int position, buffptr b);
//int is_fill(buffptr b, int i);
void load_file(buffptr b, int fd);
void free_buffer(buffptr* b);
#endif // GAP_H_INCLUDED
