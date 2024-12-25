
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

void set_input_mode(struct termios *old_termios); //gets raw terminal input without echo
void restore_input_mode(struct termios *old_termios); //resets to needing to hit return and echos input


void create_window(int startx, int starty, int HEIGHT,int WIDTH, char args[HEIGHT][WIDTH]); //creates "window" of this size with every "pixel" as a char in array

/*
 * Creates screen from a range in the buffer args between in and out, given that HEIGHT > (out - in)
 */
void create_window_inoutRANGE(int startx, int starty, int HEIGHT,int WIDTH, char ** args, int in, int out); 


/*
* Return integer code of if user input was gathered successfully
* Also modifies elapsed_time_count to get consistent time between screen updates
*/
int timed_input(double timeout_sec, double * elapsed_time_count) ;

/* to get consistent time between screen updates do:
*
*  usleep((WAITTIME*1000-elapsed_time)*1000); 
*/

void get_cursor_pos(int * row, int * col);

int countlines(FILE * f);

void copy_2d_arr(int HEIGHT, int WIDTH, int orig_height,char dest[HEIGHT][WIDTH], char source[orig_height][WIDTH], short start_range, short end_range);
 
void snap_left(char ** buffer, int * cursRow, int * cursCol,int row_offset,int col_offset);

void flush_stdin();

#endif
