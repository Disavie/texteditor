#include "Buffer.h"
#include "mytui.h"
#include "ansihelpers.h"

int get_cursor_pos(size_t *row, size_t *col) {
    char buf[64];
    int i = 0;

    // Send the ANSI escape sequence to query cursor position
    if (write(STDOUT_FILENO, "\033[6n", 4) != 4) {
        return 1;
    }

    // Read the response
    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, buf + i, 1) != 1) {
            return 1;
        }
        if (buf[i] == 'R') {
            break;
        }
        i++;
    }
    buf[i] = '\0';
    size_t oldcol = *col;
    size_t oldrow= *row;

    // Parse the response
    int tmp_row, tmp_col;
    if (buf[0] == '\033' && buf[1] == '[') {
        if (sscanf(buf + 2, "%d;%d", &tmp_row, &tmp_col) == 2) {
            *row = (size_t)tmp_row;
            *col = (size_t)tmp_col;

            // Adjust if the column matches the terminal width
            int term_rows, term_cols;
            if (get_terminal_size(&term_rows, &term_cols) == 0) {
                if (*col == (size_t)term_cols) {
                    *col = oldcol;
                }
                if (*row== (size_t)term_rows) {
                    *row = oldrow;
                }
            }
            return 0; // Success
        }
    }
    return 1; // Failure
}

int snap_left(Buffer * buf, size_t* cy, size_t * cx){
    size_t line_len = strlen(buf->contents[-1+(*cy)+buf->ypos - buf->yoffset]);
    int status = 1;
    if(line_len < (*cx)+buf->xpos-buf->xoffset){
        if(buf->xpos <= line_len){
            movecurs(*cy,line_len-buf->xpos+buf->xoffset); 
        }else{
            status = 0;
        }
    }
    get_cursor_pos(cy,cx);
    return status;
}

void snapCursorRight(Buffer * buf, size_t * cy, size_t* cx, short height, short width,short colors[]){


    char * line = buf->contents[-1+(*cy)+(buf->ypos)-buf->yoffset];
    size_t len = strlen(line);
    int moveFlag = 0;
    if(len < width){
        buf->xpos= 0;
    }else{
        buf->xpos = len-width+1;
        moveFlag = 1;
    }
    drawbuffer(0,0,height,width,buf,colors);
    if(moveFlag){ movecurs(*cy,(size_t)width+buf->xoffset-1);}
    else {movecurs(*cy,len+buf->xoffset);}

}

void snapCursorLeft(Buffer * buf,size_t * cy, size_t* cx,short height,short width,short colors[]){

    if(!snap_left(buf, cy, cx)){
        get_cursor_pos(cy,cx);
        size_t len = strlen(buf->contents[-1+(*cy)+(buf->ypos)-buf->yoffset]);
        if(len != 0){
            buf->xpos = len-1;
            *cx = buf->xoffset+1;
        }else{
            buf->xpos = len;
            *cx = buf->xoffset;
        }
        drawbuffer(0,0,height,width,buf,colors);
        movecurs(*cy,*cx);
    }
}

int smart_moveup(Buffer * buf,size_t cy ){
    int pady = 4;
    if(buf->ypos == 0 && cy-1 > buf->yoffset){
        moveup();
    }else if(buf->ypos != 0){
        if(cy-1-pady > buf->yoffset){
            moveup(); 
        }else{
            buf->ypos--;
            return 1;
        }
    }
    return 0;
}

int smart_movedown(Buffer * buf,size_t cy,int flag, int height){

    if(buf->ypos+cy-buf->yoffset < buf->linecount){
        if(cy >= height-4 && (flag || buf->ypos + cy != buf->linecount)){ 
            buf->ypos++;
            return 1;
        }else{
            movedown();
        }
    }
    return 0;

}
int smart_moveleft(Buffer * buf,size_t cx){

    if(cx > buf->xoffset){
        moveleft(); 
        return 0;
    }else if(buf->xpos != 0){
        buf->xpos--;
        return 1;
    }
    return 2;

}

int smart_moveright_n(Buffer * buf,size_t cx, size_t length,int width){


    if(cx == width-4 && buf->xpos + cx - buf->xpos < length){
        buf->xpos++;
        return 1;
    }

    if(buf->xpos +cx- buf->xoffset < length-1){
        moveright();
        return 0;
    }
    return 2;
}

int smart_moveright_i(Buffer * buf,size_t cx, size_t length,int width){

    if(cx == width - 1 -4 && buf->xpos + cx - buf->xpos < length){
        buf->xpos++;
        return 1;
    }

    if(buf->xpos +cx- buf->xoffset <= length-1){
        moveright();
        return 0;
    }
    return 2;
}


int cursorMovement(Buffer * buf,char key, size_t cy, size_t cx,char mode, int height, int width){

    switch (key){
        case 'A':
            if(smart_moveup(buf,cy))
                return 1;
            break;
        case 'B':
            if(smart_movedown(buf,cy,0,height))
                return 1;
            break;

        case 'C':
            ;
            int i;
            if(mode == 'i'){
                i = smart_moveright_i(buf,cx,strlen(buf->contents[-1+cy+buf->ypos-buf->yoffset]),width);
            }else{
                i = smart_moveright_n(buf,cx,strlen(buf->contents[-1+cy+buf->ypos-buf->yoffset]),width);
            }

            if (i) return 1;
            break;

        case 'D':
            if(smart_moveleft(buf,cx))
                return 1;
            break;
        default:
            ;
    }
    return 0;






}
