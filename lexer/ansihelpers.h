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
#define setBgColor(color) do { \
    if (color == -1) { \
        printf("\033[49m"); \
    } else { \
        printf("\033[48;5;%dm", color); \
    } \
} while (0)
#define setTextColor(color) printf("\033[38;5;%dm",color);
#define resetColor() printf("\033[0m");
#define movecurs(row, col){ printf("\033[%lu;%luH",row,col);}
#endif
