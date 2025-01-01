#include "mytui.h"
#include "ansihelpers.h"
#include "Buffer.h" 
#include "cursor.h"


int mimic_vim_w(Buffer * buf ,size_t cy, size_t*cx, short width){

    int updated = 0;
    logLine("looking at line:\n");
    logLine(buf->contents[buf->ypos+cy-buf->yoffset]);

    if((*cx)+(buf->xpos)-buf->xoffset >= strlen(buf->contents[(buf->ypos)+(cy)-buf->yoffset])-1) return 0;
    while(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset] != ' '){
        if((*cx) == width-4){
            (buf->xpos)++;
            updated = 1;
        }else{
            (*cx)++;
        }
        if((*cx)+(buf->xpos)-buf->xoffset >= strlen(buf->contents[(buf->ypos)+(cy)-buf->yoffset])-1) break;
    }
    movecurs((size_t)cy+1,(size_t)*cx);
    smart_moveright_n(buf,*cx,strlen(buf->contents[(cy)+(buf->ypos)-buf->yoffset]),width);

    return updated;
}

int mimic_vim_b(Buffer * buf ,size_t cy, size_t*cx, short width){

    int update_made = 0;

    if((buf->xpos)+(*cx)-buf->xoffset == 0)return 0;

    if((*cx) == buf->xoffset) {(buf->xpos)--;}
    else {(*cx)--;}

    if(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset] == ' '){

        if((*cx) == buf->xoffset) {(buf->xpos)--;}
        else {(*cx)--;}

    }

    while(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset] != ' '){
        if((*cx) == buf->xoffset && (buf->xpos) != 0){
            (buf->xpos)--;
            update_made = 1;
        }else{
            (*cx)--;
        }

        if((*cx)+(buf->xpos)-buf->xoffset == 0) {  break; };
    }
    movecurs((size_t)(cy)+1,(size_t)(*cx));
    if((*cx)+(buf->xpos)-buf->xoffset != 0)
        smart_moveright_n(buf,*cx,strlen(buf->contents[(cy)+(buf->ypos)-buf->yoffset]),width);
    return update_made;

}



