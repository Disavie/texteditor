#ifndef COLORSCHEME
#define COLORSCHEME

#include "stdlib.h"


typedef struct{
    const char * name;
    const void * data;

}Theme;

extern const Theme zthemes[];

extern const size_t zthemecount;


#endif
