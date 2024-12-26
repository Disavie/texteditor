#ifndef ANSIHELPER
#define ANSIHELPER

#include <stdio.h>

#define moveup() printf("\033[A");
#define movedown() printf("\033[B");
#define moveright() printf("\033[C");
#define moveleft() printf("\033[D");
#define clear_screen() printf("\033[2J");
#define move00() printf("\033[H");
#define hidecursor() printf("\033[?25l");
#define showcursor() printf("\033[?25h");
#define openAltBuffer() printf("\033[?1049h");
#define closeAltBuffer() printf("\033[?1049l");


#endif
