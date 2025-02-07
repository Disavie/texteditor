#ifndef MYTUI 
#define MYTUI 


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <locale.h>
#include <unistd.h>
#include <wchar.h>
#include <termios.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>
#include <sys/time.h>
#include <time.h>
#include "Buffer.h"

//stop CTRL+C/X/\ from killing
void handle_signal(int d);


//ALWAYS LOGS TO A FILE CALLED main.log
void logChar(char ch);
void logLine(char * line);
void logNum(double num);
void clearLog();

void trim_trailing_spaces(char *str);
void set_input_mode(struct termios *old_termios); //gets raw terminal input without echo
void restore_input_mode(struct termios *old_termios); //resets to needing to hit return and echos input
int get_terminal_size(int *rows, int *cols) ;

char* size_t_to_string(size_t num) ;
double get_time_ms();
void get24Htime(char * put,size_t size);
size_t countLines(FILE * f);

// --DRAWING TO SCREEN FUNCTIONS --
void drawLogo(int HEIGHT,int WIDTH, const short color[]);

void strcpyf(char *destination, size_t dest_size, const char *format, ...) ;
int getnum(char * line);


//VIMLIKE MOTIONS
int mimic_vim_w(Buffer * buf,size_t  cy, size_t *cx, short width);
int mimic_vim_b(Buffer * buf ,size_t  cy,size_t *cx, short width);


char updateMode(char inputch, char *mode,char ** modestr);


char * highlightLine(char *line,const short colors[]);
char * highlightsubstr(char *line,size_t start, size_t end, const short colors[]);
char * substr(char * line, size_t start, size_t end);
char * substr_2(char * start, size_t end);
size_t calcsizeKB(Buffer * buf); //this works for KB
size_t calcsize(Buffer * buf);
size_t calcbytes(char * filename); //most accurate
#endif


