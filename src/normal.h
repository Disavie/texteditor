#ifndef NORMAL
#define NORMAL
#include "Buffer.h"

int normalmain(char ch,Buffer ** buf,size_t * cy, size_t * cx, short winheight,short winwidth, Buffer *** history, void * extras[], const short * colors[]);

//VIMLIKE MOTIONS
int mimic_vim_w(Buffer * buf,size_t  cy, size_t *cx, short width);
int mimic_vim_b(Buffer * buf ,size_t  cy,size_t *cx, short width);


#endif
