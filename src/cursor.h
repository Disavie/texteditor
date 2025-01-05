#ifndef CURSMOVER
#define CURSMOVER
#include "mytui.h"


int get_cursor_pos(size_t* row, size_t* col);

int snap_left(Buffer * buf, size_t* cy, size_t * cx);
void snapCursorLeft(Buffer * buf,size_t * cy, size_t* cx,short height,short width,const short colors[]);
void snapCursorRight(Buffer * buf, size_t * cy, size_t* cx, short height, short width,const short colors[]);

int smart_moveup(Buffer * buf,size_t cy );
int smart_movedown(Buffer * buf,size_t cy, int height);
int smart_moveleft(Buffer * buf,size_t cx);
int smart_moveright_n(Buffer * buf,size_t cx, size_t length,int width);
int smart_moveright_i(Buffer * buf,size_t cx, size_t length,int width);
int cursorMovement(Buffer * buf,char key, size_t cx, size_t cy,char mode, int height, int width);

void centeronline(Buffer * buf,size_t cy, size_t cx, short height);


#endif
