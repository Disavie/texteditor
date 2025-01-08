#include "mytui.h"
#include "ansihelpers.h"
#include "Buffer.h" 
#include "cursor.h"
#include "normal.h"
#include "colorscheme.h"
#include <unistd.h>

#define STATUSBARHEIGHT 2


//cursorMovement(buf,input_buffer[2],cy,cx,mode,WINHEIGHT, winwidth)
int normalmain(char input,Buffer ** buf,size_t * cy, size_t * cx, short winheight,short winwidth, Buffer *** history, void * extras[], const short * colors[]) {
    
    /*
    //varibles in normalmain 
    //&mbuf &cy &cx WINHEIGHT, WINWIDTH &undohistory
    //extras : &statusBarMsg, sblen, &modestr, &mode, &filesize, &hasSaved, &lastsavetimestr, &statusbarpos
    //colors
    //                 char *       s_t *  char **  char * size_t * int *     char *          short *
    void * extras[] = {statusBarMsg,&sblen,&modestr,&mode,&filesize,&hasSaved,lastsavetimestr,&STATUSBARPOS,

                       size_t * size_t * int *
                       &his_i,&his_sz,&made_edit 
    };
    */

    char * statusBarMsg = (char *)extras[0];
    size_t sblen = *(size_t *)extras[1];
    char * modestr = *(char **)extras[2];
    char *mode = (char *)extras[3];
    size_t * filesize = (size_t *)extras[4];
    int * hasSaved = (int *)extras[5];
    char * lastsavetimestr = (char *)extras[6];
    short sbarpos = *(short *)extras[7];
    size_t * his_i = (size_t *)extras[8];
    size_t * his_sz = (size_t *)extras[9];
    int * made_edit = (int *)extras[10];
    
    char ch = input;

        if(input == '/') {

            size_t oldRow = *cy;
            size_t oldCol = *cx;
            char cmdbuf[128];
            memset(cmdbuf,'\0',sizeof(cmdbuf));
            int cmdi = 0;
            cmdbuf[cmdi] = ch;
            update_statusbar(cmdbuf,sbarpos,winwidth,(*buf),(*colors),*cy,*cx);
            showcursor();
            movecurs((size_t)sbarpos+(*buf)->yoffset+1,(size_t)2);
            cmdi++;
            char ibuf[8];
            int nocheck = 0;
            ssize_t bytes_read = 0;
            while((bytes_read = read(STDIN_FILENO,ibuf,sizeof(ibuf))) != -1){
                if(ibuf[0] == '\n') break;
                if(ibuf[0] == '\033') {nocheck = 1; break;}
                if(ibuf[0] == '\t') continue;

                if((short)ibuf[0] == 127){
                    cmdbuf[cmdi-1] = '\0';
                    cmdi--;
                    if(cmdi == 0){ nocheck = 1; break; }
                }else{
                    cmdbuf[cmdi] = ibuf[0];
                    cmdi++;
                }
                update_statusbar(cmdbuf,sbarpos,winwidth,(*buf),(*colors),oldRow,oldCol);
                movecurs((size_t)winheight+STATUSBARHEIGHT,strlen(cmdbuf)+1);
            }   
            if(!nocheck){ 
                trim_trailing_spaces(cmdbuf);

                if(strcmp(cmdbuf,"/w") == 0){
                    if((*buf)->filename == NULL){
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed file, try /saveas",(*colors)[5]);
                    }else{
                        get24Htime(lastsavetimestr,64);
                        *filesize = calcsize((*buf));
                        clearbarr(history,his_sz);
                        *his_i = 0;
                        saveFile((*buf));
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,*filesize,lastsavetimestr);
                        *hasSaved = 1;
                    }
                } else if(strcmp(cmdbuf,"/wq")== 0){
                    if((*buf)->filename != NULL){
                        get24Htime(lastsavetimestr,64);
                        saveFile((*buf));
                        *filesize = calcsize((*buf));
                        clearbarr(history,his_sz);
                        *his_i = 0;
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,*filesize,lastsavetimestr);
                        *hasSaved = 1;
                        return 1;
                    }else{
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed file, try /saveas",(*colors)[5]);
                    }

                }else if(strcmp(cmdbuf, "/q")==0){
                    if(*hasSaved || (*buf)->filename == NULL){
                        return 1;
                    }else{
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnsaved changes, try /w or /q! to quit without saving",(*colors)[5]);
                    }
                }else if(strcmp(cmdbuf,"/q!")==0){
                    return 1;
                }   
                else if(strncmp(cmdbuf,"/saveas ",8) == 0){
                    char * newname = cmdbuf+8;
                    char * oldfilename = (*buf)->filename;
                    char * new = (char *)malloc(strlen(newname)*sizeof(char));
                    strcpy(new,newname);
                    (*buf)->filename = new;
                    if(oldfilename != NULL) free(oldfilename);
                    get24Htime(lastsavetimestr,64);
                    *filesize = calcsize((*buf));
                    *his_i = 0;
                    clearbarr(history,his_sz);
                    saveFile((*buf));
                    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,*filesize,lastsavetimestr);
                    *hasSaved = 1;

                }else if(strncmp(cmdbuf,"/e! ",4) == 0){
                    char * newname = cmdbuf+4;
                    loadFile((*buf),newname);
                    oldRow = (*buf)->yoffset;
                    oldCol = (*buf)->xoffset;
                    (*buf)->ypos = 0;
                    (*buf)->xpos = 0;
                    clearbarr(history,his_sz);
                    addbuffer(history,his_sz,(*buf));
                    *his_i = 0;

                }else if(strncmp(cmdbuf,"/e ",3) == 0){
                    if(*hasSaved){
                        char * newname = cmdbuf+3;
                        loadFile((*buf),newname);
                        oldRow = (*buf)->yoffset;
                        oldCol = (*buf)->xoffset;
                        (*buf)->ypos = 0;
                        (*buf)->xpos = 0;
                        clearbarr(history,his_sz);
                        addbuffer(history,his_sz,(*buf));
                        *his_i = 0;

                    }else{
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnsaved changes, try /w or /e! to edit without saving",(*colors)[5]);
                    }

                }else if(strncmp(cmdbuf, "/theme ",7) == 0){
                    char * newtheme = cmdbuf+7; 
                    int changed = 0;
                    for(int i = 0 ; i < zthemecount ; i++) {
                        if(strcmp(newtheme,zthemes[i].name) == 0){
                            (*colors) = zthemes[i].data;
                            changed = 1;
                            break;
                        }
                    }
                    if(!changed){
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmNo such theme as \'%s\'",(*colors)[5],newtheme);
                    }

                }else{
                    strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnknown command \'%s\'",(*colors)[5],cmdbuf);
                }
            }
            memset(cmdbuf,'\0',sizeof(cmdbuf));
            cmdi = 0;
            *cy = oldRow;
            *cx = oldCol;
            movecurs(*cy,*cx);
            hidecursor();
    }else{

        //chainable actions will end in 
        //f<letter>, hjkl, d*
        //oO HJKL a u r are NOT chainable
        //
        //* only if it is "dd" 
        //
        //command + count + motion
        //commands: d*, x, 
        //count <number>
        //motion hjkl fF w b
        //build motion
        char motionbuffer[32]; //You are a freak if whatever motion shortcut you want is >32 characters
        memset(motionbuffer,'\0',32);

        int mbi =  0;
        if(ch == 19 || ch == 17 || ch == 's' || ch == 'c' || ch == 'H' || ch == 'J' || ch == 'K' || ch == 'L' || ch == 'o' || ch == 'O' || ch == 'a' || ch == 'u' || ch == 'r'){
            motionbuffer[0] = ch;
        } else {
            ssize_t bytes_read = 0;
            char i_buf[8];
            i_buf[0] = ch;
            do {
                char cur = i_buf[0];
                motionbuffer[mbi] = cur; 
                if(mbi != 0 && motionbuffer[mbi-1] == 'd' && cur == 'd'){ // "dd"
                    logLine("\nbreaking condition 1 motionbuffer = ");logLine(motionbuffer);
                    break;
                } else if ( cur == 'x' || cur == 'h' || cur == 'j' || cur == 'k' || cur == 'l' ||  cur == 'w' || cur == 'b') {
                    logLine("\nbreaking condition 2 motionbuffer = ");logLine(motionbuffer);
                    break;
                }else if(cur == 'F' || cur == 'f'){
                    mbi++;
                    char x = getchar();
                    motionbuffer[mbi] = x;
                    logLine("\nbreaking condition 3  motionbuffer = ");logLine(motionbuffer);
                    mbi--;
                    break;
                }

                mbi++;
            } while ( mbi < 32 && (bytes_read = read(STDIN_FILENO,i_buf,8)) != -1);
        }
         
        int count = getnum(motionbuffer);
        logLine("\n");logNum(count);
        int loop = 0;
        int starty = (*buf)->ypos + (*buf)->yoffset + (*cy);
        int startx = (*buf)->xpos + (*buf)->xoffset + (*cx);
        int ydist = 0; 
        int xdist = 0;

        while(mbi > -1) {
            ch = motionbuffer[mbi];
            while(loop < count) {
                logLine("\n mbi = ");logNum(mbi);
                get_cursor_pos(cy,cx);
                switch(ch){


                    //MOVEMENT COMMANDS
                    case 'h':
                        cursorMovement((*buf),'D',*cy,*cx,*mode,winheight, winwidth);
                        break;
                    case 'j':
                        cursorMovement((*buf),'B',*cy,*cx,*mode,winheight, winwidth);
                        break;
                    case 'k':
                        cursorMovement((*buf),'A',*cy,*cx,*mode,winheight, winwidth);
                        break;
                    case 'l':
                        cursorMovement((*buf),'C',*cy,*cx,*mode,winheight, winwidth);
                        break;
                    case 'H': 
                        (*buf)->xpos= 0;
                        *cx = (*buf)->xoffset + (*buf)->xcorner;
                        movecurs(*cy,*cx);
                        break;
                    case 'J':
                        for(int i = 0 ; i < 24 ; i++) {
                            smart_movedown((*buf),*cy,winheight);
                            get_cursor_pos(cy, cx);
                        }
                        centeronline((*buf),*cy,*cx,winheight);
                        break;
                    case 'K':
                        for(int i = 0 ; i < 24 ; i++) {
                            smart_moveup((*buf),*cy);
                            get_cursor_pos(cy, cx);
                        }
                        centeronline((*buf),*cy,*cx,winheight);
                        break;
                    case 'L': 
                        while((*buf)->xpos + *cx - (*buf)->xoffset < strlen((*buf)->contents[(*buf)->ypos + *cy - (*buf)->yoffset])-1){
                            smart_moveright_n((*buf),*cx,strlen((*buf)->contents[(*buf)->ypos + *cy - (*buf)->yoffset]),winwidth);
                            get_cursor_pos(cy,cx);
                        }

                        break;
                    case 'w':
                        mimic_vim_w((*buf),*cy,cx,winwidth);
                        break;
                    case 'b':
                        mimic_vim_b((*buf),*cy,cx,winwidth);
                        break;
                    case 'f':
                        ;
                        char temp = motionbuffer[mbi+1];
                        short found = 0;
                        size_t temp2= *cx;
                        size_t temp3= (*buf)->xpos;


                        smart_moveright_n((*buf),*cx,strlen((*buf)->contents[(*cy)+(*buf)->ypos-(*buf)->yoffset]),winwidth);
                        get_cursor_pos(cy,cx);
                        while((*buf)->xpos+(*cx)-(*buf)->xoffset < (int)strlen((*buf)->contents[(*buf)->ypos+(*cy)-(*buf)->yoffset])){
                            logLine("\nlooping pooping");
                            if((*buf)->contents[(*buf)->ypos+(*cy)-(*buf)->yoffset][(*buf)->xpos+(*cx)-(*buf)->xoffset] == temp){
                                found = 1;
                                break;
                            }
                            if(smart_moveright_n((*buf),*cx,strlen((*buf)->contents[(*cy)+(*buf)->ypos-(*buf)->yoffset]),winwidth) == 2)
                                break;
                            get_cursor_pos(cy,cx);
                        }
                        if(!found){
                            (*buf)->xpos = temp3;
                            *cx = temp2; 
                            movecurs(*cy,*cx);
                        }
                        break;
                    case 'F':
                        ;
                        temp = motionbuffer[mbi+1];
                        found = 0;
                        temp2= *cx;
                        temp3 = (*buf)->xpos;


                        smart_moveleft((*buf),*cx);
                        get_cursor_pos(cy,cx);

                        while((int)((*buf)->xpos)+(int)(*cx)-(int)((*buf)->xoffset) > -1){
                            if((*buf)->contents[(*buf)->ypos+(*cy)-(*buf)->yoffset][(*buf)->xpos+(*cx)-(*buf)->xoffset] == temp){
                                found = 1;
                                break;
                            }
                            smart_moveleft((*buf),*cx);
                            get_cursor_pos(cy,cx);
                            if((*buf)->xpos+(*cx)-(*buf)->xoffset == 0) break;
                        }
                        if((*buf)->contents[(*buf)->ypos+(*cy)-(*buf)->yoffset][(*buf)->xpos+(*cx)-(*buf)->xoffset] == temp){
                            found = 1;
                        }
                        if(!found){
                            *cx= temp2; 
                            (*buf)->xpos = temp3;
                            movecurs(*cy,*cx);
                        }
                        break;
                    case 'c':
                        ; 
                        centeronline((*buf),*cy,*cx,winheight);

                        break;


                    //EDIT COMMANDS 
                    case 'd':
                        if((*buf)->linecount == 1) {
                            char * newline = (char *)malloc(sizeof(char));
                            newline[0] = '\0';
                            void * oldline = (*buf)->contents[0];
                            (*buf)->contents[0] = newline;
                            free(oldline);
                        }else{
                            remove_line((*buf),(*buf)->ypos+(*cy)-(*buf)->yoffset);
                            if((*buf)->ypos+(*cy)-(*buf)->yoffset == (*buf)->linecount) smart_moveup(*buf,*cy);
                        }

                        break;
                    case 'a':
                        smart_moveright_i((*buf),*cx,strlen((*buf)->contents[(*cy)+(*buf)->ypos-(*buf)->yoffset]),winwidth);
                        get_cursor_pos(cy,cx);
                        updateMode('i',mode,&modestr); 
                        strcpyf(statusBarMsg,sblen,"%s",modestr);
                        update_statusbar(statusBarMsg,sbarpos,winwidth,(*buf),(*colors),*cy,*cx);
                        *hasSaved = 0;
                        *made_edit = 1;
                        break;
                    case 'x':
                        remove_from_line((*buf),(*buf)->ypos+(*cy)-(*buf)->yoffset,(*buf)->xpos+(*cx)-(*buf)->xoffset);
                        *hasSaved = 0;
                        *made_edit = 1;
                        break;
                    case 's':
                        remove_from_line((*buf),(*buf)->ypos+(*cy)-(*buf)->yoffset,(*buf)->xpos+(*cx)-(*buf)->xoffset);
                        *hasSaved = 0;
                        updateMode('i',mode,&modestr); 
                        strcpyf(statusBarMsg,sblen,"%s",modestr);
                        update_statusbar(statusBarMsg,sbarpos,winwidth,(*buf),(*colors),*cy,*cx);
                        *made_edit = 1;
                        break;
                    case 'o':
                        ;    
                        insert_line((*buf),"",1+(*buf)->ypos+(*cy)-(*buf)->yoffset);
                        smart_movedown((*buf),*cy,winheight);
                        get_cursor_pos(cy,cx);
                        movecurs(*cy,(size_t)(*buf)->xoffset);
                        *hasSaved = 0;
                        updateMode('i',mode,&modestr); 
                        //      char *      size_t      char *
                        strcpyf(statusBarMsg,sblen,"%s",modestr);
                        update_statusbar(statusBarMsg,sbarpos,winwidth,(*buf),(*colors),*cy,*cx);
                        *made_edit = 1;
                        break;
                    case 'O':
                        ;    
                        insert_line((*buf),"",(*buf)->ypos+(*cy)-(*buf)->yoffset);
                        get_cursor_pos(cy,cx);
                        movecurs(*cy,(size_t)(*buf)->xoffset);

                        *hasSaved = 0;
                        updateMode('i',mode,&modestr); 
                        strcpyf(statusBarMsg,sblen,"%s",modestr);
                        update_statusbar(statusBarMsg,sbarpos,winwidth,(*buf),(*colors),*cy,*cx);
                        *made_edit = 1;
                        break;



                    //MISC COMMANDS
                    case 'u':
                        if(*his_i > 0){
                            (*his_i)--; 
                            freeBuffer(*buf);
                            Buffer * new = malloc(sizeof(Buffer));
                            copyBuffer(new,(*history)[*his_i]);
                            (*buf) = new;
                        }else if(*his_i == 0 ){
                            strcpyf(statusBarMsg,sblen,"\033[38;5;%dmAlready at earliest change",(*colors)[5]);
                        }
                        break;
                    case 'r':
                        if(*his_i < *his_sz - 1){
                            (*his_i)++; 
                            freeBuffer(*buf);
                            Buffer * new = malloc(sizeof(Buffer));
                            copyBuffer(new,(*history)[*his_i]);
                            (*buf) = new;
                        }else if(*his_i == *his_sz - 1 ){
                            strcpyf(statusBarMsg,sblen,"\033[38;5;%dmAlready at newest change",(*colors)[5]);
                        }
                        break;
                    case 17: //CTRL + Q
                        return 1;
                    case 19: //CTRL + S
                        if((*buf)->filename != NULL){
                            get24Htime(lastsavetimestr,64);
                            *filesize = calcsize((*buf));
                            clearbarr(history,his_sz);
                            saveFile((*buf));
                            *his_i = 0;
                            strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,*filesize,lastsavetimestr);
                            *hasSaved = 1;
                        }else{
                            strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed File, try /saveas",(*colors)[5]);
                        }
                        break;
                    default:
                        ;
                }
                loop++;
            }
            count = 1; 
            loop = 0;
            mbi--;
        }
    }
    return 0;
}

int mimic_vim_w(Buffer * buf ,size_t cy, size_t*cx, short width){

    int updated = 0;

    if((*cx)+(buf->xpos)-buf->xoffset >= strlen(buf->contents[(buf->ypos)+(cy)-buf->yoffset])-1) return 0;

    while(isalnum(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset])){
        if((*cx) == width-10){
            (buf->xpos)++;
            updated = 1;
        }else{
            (*cx)++;
        }
        if((*cx)+(buf->xpos)-buf->xoffset >= strlen(buf->contents[(buf->ypos)+(cy)-buf->yoffset])-1) break;
    }
    movecurs((size_t)cy,(size_t)*cx);
    smart_moveright_n(buf,*cx,strlen(buf->contents[(cy)+(buf->ypos)-buf->yoffset]),width);

    return updated;
}

int mimic_vim_b(Buffer * buf ,size_t cy, size_t*cx, short width){

    int update_made = 0;

    if((buf->xpos)+(*cx)-buf->xoffset == 0)return 0;

    if((*cx) == buf->xoffset) {(buf->xpos)--;}
    else {(*cx)--;}

    if(!isalnum(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset])){
        if((*cx) == buf->xoffset) {(buf->xpos)--;}
        else {(*cx)--;}
    }

    while(isalnum(buf->contents[(buf->ypos)+(cy)-buf->yoffset][(buf->xpos)+(*cx)-buf->xoffset])){
        if((*cx) == buf->xoffset && (buf->xpos) != 0){
            (buf->xpos)--;
            update_made = 1;
        }else{
            (*cx)--;
        }

        if((*cx)+(buf->xpos)-buf->xoffset == 0) {  break; };
    }
    movecurs((size_t)(cy),(size_t)(*cx));
    if((*cx)+(buf->xpos)-buf->xoffset != 0)
        smart_moveright_n(buf,*cx,strlen(buf->contents[(cy)+(buf->ypos)-buf->yoffset]),width);
    return update_made;

}

