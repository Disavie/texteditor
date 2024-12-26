#ifndef ANSIHELPER
#define ANSIHELPER

#define moveup() printf("\033[A");
#define movedown() printf("\033[B");
#define moveright() printf("\033[C");
#define moveleft() printf("\033[D");
#define clear_screen() printf("\033[2J");
#define move00() printf("\033[H");
 
#endif
