
#ifndef MYTUI 
#define MYTUI 


#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include <unistd.h>
#include <wchar.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ctype.h>



//ALWAYS LOGS TO A FILE CALLED main.log
void logChar(char ch);
void logLine(char * line);
void logNum(double num);
void clearLog();

void loadFile(char *** dest,char * filename,size_t * linecount, size_t * longestLine, size_t * szHelper,size_t HEIGHT);
void createFile(char *** dest,char * filename,size_t * linecount, size_t * longestLine, size_t * szHelper,size_t HEIGHT);

void set_input_mode(struct termios *old_termios); //gets raw terminal input without echo
void restore_input_mode(struct termios *old_termios); //resets to needing to hit return and echos input

/*
* Return integer code of if user input was gathered successfully
* Also modifies elapsed_time_count to get consistent time between screen updates
*/
int timed_input(double timeout_sec, double * elapsed_time_count) ;

int get_cursor_pos(int * row, int * col);

size_t countLines(FILE * f);
size_t countLongestLine(FILE * f);
size_t countLongestLineBuffer(char ** buffer, size_t length);

int snap_left(char** buffer, int * cursRow, int * cursCol, int *yIn, int *xIn, short yOffset, short xOffset); 
int snap_right(char * line, int cRow, short xOffset);
void snapCursorLeft(char ** f_buf, int * cRow, int  * cCol, int * yStart, int * xStart, int yOffset, int xOffset,int rend_HEIGHT,int WIDTH,size_t linecount,short colors[]);
void snapCursorRight(char ** f_buf, int * cRow, int * cCol, int * yStart, int* xStart, int yOffset, int xOffset, int rend_HEIGHT, int rend_WIDTH, size_t linecount,short colors[]);


char * insert_to_line(char ** buf, char * line, int buf_row, int index_in_line,char ch); 
char * remove_from_line(char ** buf, char * line, int buf_row, int index);
char *insert_line(char ***buf, char *line, int buf_row, size_t *buf_height) ;
char * remove_line(char *** buf, int buf_row, size_t *buf_height);

int smart_moveup(int cRow,int *yStart,short yOffset);
int smart_movedown(int cRow, int *yStart,  int flag,size_t linecount, int rend_HEIGHT);
int smart_moveleft(int cCol, int *xStart, short xOffset);
int smart_moveright2(int cCol, int *xStart, int xOffset,size_t length,int WIDTH);
int smart_moveright2_insert(int cCol, int *xStart, int xOffset,size_t length,int WIDTH);

// --DRAWING TO SCREEN FUNCTIONS --
void drawLogo(int HEIGHT,int WIDTH, short color[]);
/*
 * Creates screen from a range in the buffer args between in and out
 */
void create_window_inoutRANGE(int startx, int starty, int HEIGHT,int WIDTH, char ** args,int yIn, int xIn,size_t linecount, short colors[]); 
void drawStatusBar(char * text,int width, short colors[],short isError);
void update_statusbad(char * text,int widith, int height, char ** modes, char mode, int cRow, int cCol, int yOffset,char * filename,short colors[],short isError);
void draw_borders(int startx, int starty, int win_height, int win_width);




char updateMode(char inputch, char *mode);
int saveFile(char * filename,size_t linecount,char *** f_buf);
void trim_trailing_spaces(char *str);

//VIMLIKE MOTIONS

int mimic_vim_w(char *** f_buf, int * yStart, int * xStart,int * cRow, int *cCol, int yOffset, int xOffset, int rend_WIDTH);
int mimic_vim_b(char *** f_buf, int * yStart, int * xStart, int * cRow, int * cCol, int yOffset, int xOffset,int rend_WIDTH);


#endif


