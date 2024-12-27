
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


typedef struct{

    int x;
    int y;
    float xvel;
    float yvel;

}Player;


//ALWAYS LOGS TO A FILE CALLED main.log
void logChar(char ch);
void logLine(char * line);
void logNum(double num);
void clearLog();

void set_input_mode(struct termios *old_termios); //gets raw terminal input without echo
void restore_input_mode(struct termios *old_termios); //resets to needing to hit return and echos input


void create_window(int startx, int starty, int HEIGHT,int WIDTH, char args[HEIGHT][WIDTH]); //creates "window" of this size with every "pixel" as a char in array

/*
 * Creates screen from a range in the buffer args between in and out, given that HEIGHT > (out - in)
 */
void create_window_inoutRANGE(int startx, int starty, int HEIGHT,int WIDTH, char ** args,int yIn, int xIn); 
void noflicker_create_window_inoutRANGE(int startx, int starty, int win_height, int max_width, char **args, int yIn, int xIn);

void draw_borders(int startx, int starty, int win_height, int win_width);

/*
* Return integer code of if user input was gathered successfully
* Also modifies elapsed_time_count to get consistent time between screen updates
*/
int timed_input(double timeout_sec, double * elapsed_time_count) ;

/* to get consistent time between screen updates do:
*
*  usleep((WAITTIME*1000-elapsed_time)*1000); 
*/

int get_cursor_pos(int * row, int * col);

size_t countLines(FILE * f);
size_t countLongestLine(FILE * f);
size_t countLongestLineBuffer(char ** buffer, size_t length);

void copy_2d_arr(int HEIGHT, int WIDTH, int orig_height,char dest[HEIGHT][WIDTH], char source[orig_height][WIDTH], short start_range, short end_range);
 
int snap_left(char** buffer, int * cursRow, int * cursCol, int yIn, int xIn, int yOffset, int xOffset); 

void flush_stdin();

char * insert_to_line(char ** buf, char * line, int buf_row, int index_in_line,char ch); 
char * remove_from_line(char ** buf, char * line, int buf_row, int index);

//will either scroll or move the cursor,, return value is a flag 1 = scroll 0 = moved in x direction

int smart_moveup(int cRow,int *yStart);
int smart_movedown(int cRow, int *yStart, int linecount, int rend_HEIGHT);
int smart_moveleft(int cCol, int *xStart);
int smart_moveright(int cCol, int *xStart, int longestline, int WIDHT);
#endif
