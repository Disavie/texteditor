#ifndef BUFFER
#define BUFFER

#include "ansihelpers.h"

typedef struct{

    char * filename;
    char ** contents;
    size_t linecount;

    //values for when being rendered on screen
    short yoffset;
    short xoffset;
    size_t xpos;
    size_t ypos;



}Buffer;

char * insert_to_line(Buffer * buf, size_t row, size_t index_in_line,char ch);
char * remove_from_line(Buffer * buf, size_t row, size_t index) ;
char *insert_line(Buffer * buf, char * line,size_t index) ;
char *remove_line(Buffer * buf, int buf_row) ;

void loadFile(Buffer * dest,char * filename); 
void createFile(Buffer * dest,char * filename);
int saveFile(Buffer * buf);
void drawbuffer(short starty, short startx, int win_height,int win_width, Buffer * buffer,const short colors[]); 

void drawStatusBar(char * text,int width);
void update_statusbar(char * words,short ypos, short width, char ** modes, char mode, Buffer * buf,const short colors[],short isError, size_t cy, size_t cx);

#endif
