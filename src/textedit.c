#include <unistd.h>
#include "mytui.h"
#include "Buffer.h"
#include "cursor.h"
#include "ansihelpers.h"
#include "colorscheme.h"

int main(int argc, char ** argv){
    openAltBuffer();

    struct termios oldtermios;
    set_input_mode(&oldtermios);
    setbuf(stdout,NULL);
    //signal(SIGINT,handle_signal);
    signal(SIGTSTP,handle_signal);
    signal(SIGQUIT,handle_signal);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    short WIDTH = w.ws_col;
    short HEIGHT = w.ws_row;


    int hasSaved = 1;
    char filename[32]; 
    Buffer mBuf = {
        .filename = NULL,
        .contents = NULL,
        .xoffset = 7,
        .yoffset = 1,
        .xcorner = 0,
        .ycorner = 0,
        .ypos = 0,
        .xpos = 0,
    };

    if(argc > 1){
        loadFile(&mBuf,argv[1]);
    }else{
        createFile(&mBuf,NULL);
    }

    short STATUSBARHEIGHT = 2;
    char statusBarMsg[128] = "";
    size_t sblen = 128;
    size_t cy = mBuf.yoffset+mBuf.ycorner;
    size_t cx = mBuf.xoffset+mBuf.xcorner;
    short update_made = 1;
    char ch = '\0';
    char mode = 'n'; 

    const short * colors = light;

    char temp[128];

    short isError = 0;
    short WINHEIGHT = HEIGHT - STATUSBARHEIGHT;
    short WINWIDTH = WIDTH;

    int lastsavesize= 0;
    char lastsavetimestr[64];
    get24Htime(lastsavetimestr,64);
    char * modestr = (char *)malloc(65*sizeof(char));
    strcpy(modestr,"--NORMAL--");
    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);


    drawbuffer(mBuf.ycorner,mBuf.xcorner,WINHEIGHT,WINWIDTH,&mBuf,colors);
    movecurs(cy,cx);
    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
    if(argc == 1){movecurs((mBuf.linecount),(size_t)0); drawLogo(HEIGHT,WIDTH, colors); }
    movecurs(cy,cx);
    showcursor();

    char input_buffer[8];
    memset(input_buffer,'\0',sizeof(input_buffer));
    int tabwidth = 4;
    //motion anatomy prefix (d y)
    //count
    //motion type (w f<char> $ ^)

    ssize_t bytes_read;
    char cmdbuf[128];
    memset(cmdbuf,'\0',sizeof(cmdbuf));
    short cmdi = 0;

    int quit = 0;
    Buffer * lastSave = copyBuffer(&mBuf);

    while((bytes_read = read(STDIN_FILENO,input_buffer,sizeof(input_buffer))) != -1){

        hidecursor();
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col;
        HEIGHT = w.ws_row;
        WINHEIGHT = HEIGHT - STATUSBARHEIGHT;
        WINWIDTH = WIDTH; 
    

        if(bytes_read == 0) continue;
        ch = input_buffer[0];

        if(updateMode(ch,&mode,&modestr)){ 
            strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize ,lastsavetimestr);
            update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
            continue;
        }
        //varibles in here
        //mBuf cy cx WINHEIGHT, WINWIDTH, statusBarMsg
        //colors
        if(mode == 'n'){
            switch((int)ch){
                case 'h':
                    input_buffer[2] = 'D';
                    goto movement;
                case 'j':
                    input_buffer[2] = 'B';
                    goto movement;
                case 'k':
                    input_buffer[2] = 'A';
                    goto movement;
                case 'l':
                    input_buffer[2] = 'C';
                    goto movement;
                case 'H': 
                    mBuf.xpos= 0;
                    cx = mBuf.xoffset + mBuf.xcorner;
                    movecurs(cy,cx);
                    update_made = 1;
                    break;
                case 'J':
                    for(int i = 0 ; i < 24 ; i++) {
                        smart_movedown(&mBuf,cy,WINHEIGHT);
                        get_cursor_pos(&cy, &cx);
                    }
                    centeronline(&mBuf,cy,cx,WINHEIGHT);
                    update_made = 1;
                    break;
                case 'K':
                    for(int i = 0 ; i < 24 ; i++) {
                        smart_moveup(&mBuf,cy);
                        get_cursor_pos(&cy, &cx);
                    }
                    centeronline(&mBuf,cy,cx,WINHEIGHT);
                    update_made = 1;
                    break;
                case 'L': 
                    while(mBuf.xpos + cx - mBuf.xoffset < strlen(mBuf.contents[mBuf.ypos + cy -  mBuf.yoffset])-1){
                        smart_moveright_n(&mBuf,cx,strlen(mBuf.contents[mBuf.ypos + cy - mBuf.yoffset]),WINWIDTH);
                        get_cursor_pos(&cy,&cx);
                    }
                    update_made = 1;
                    break;
                case 'd':
                    if(mBuf.linecount == 1) {
                        char * newline = (char *)malloc(sizeof(char));
                        newline[0] = '\0';
                        void * oldline = mBuf.contents[0];
                        mBuf.contents[0] = newline;
                        free(oldline);
                    }else{
                        remove_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset);
                        if(mBuf.ypos+cy-mBuf.yoffset == mBuf.linecount) smart_moveup(&mBuf,cy);
                    }
                    update_made = 1;
                    break;
                case 'w':
                    if(mimic_vim_w(&mBuf,cy,&cx,WINWIDTH))
                        update_made = 1;
                    break;
                case 'b':
                    if(mimic_vim_b(&mBuf,cy,&cx,WINWIDTH))
                        update_made = 1;
                    break;
                case 'a':
                    smart_moveright_i(&mBuf,cx,strlen(mBuf.contents[cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
                    get_cursor_pos(&cy,&cx);
                    updateMode('i',&mode,&modestr); 
                    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
                    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                    goto end_frame;
                case 'x':
                    remove_from_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset);
                    update_made = 1;
                    break;
                case 's':
                    remove_from_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset);
                    update_made = 1;
                    updateMode('i',&mode,&modestr); 
                    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
                    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                    goto end_frame;
                case 'o':
                    ;    
                    insert_line(&mBuf,"",1+mBuf.ypos+cy-mBuf.yoffset);
                    smart_movedown(&mBuf,cy,WINHEIGHT);
                    get_cursor_pos(&cy,&cx);
                    movecurs(cy,(size_t)mBuf.xoffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode,&modestr); 
                    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
                    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                    goto end_frame;
                case 'O':
                    ;    
                    insert_line(&mBuf,"",mBuf.ypos+cy-mBuf.yoffset);
                    get_cursor_pos(&cy,&cx);
                    movecurs(cy,(size_t)mBuf.xoffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode,&modestr); 
                    strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
                    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                    goto end_frame;
                case 'f':
                    ;
                    char temp = getchar();
                    short found = 0;
                    size_t temp2= cx;
                    size_t temp3= mBuf.xpos;
                    update_made = 1;

                    smart_moveright_n(&mBuf,cx,strlen(mBuf.contents[cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
                    get_cursor_pos(&cy,&cx);
                    while(mBuf.xpos+cx-mBuf.xoffset < (int)strlen(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset])){
                        if(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                            found = 1;
                            break;
                        }
                        smart_moveright_n(&mBuf,cx,strlen(mBuf.contents[cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
                        get_cursor_pos(&cy,&cx);
                    }
                    if(!found){
                        mBuf.xpos = temp3;
                        cx= temp2; 
                        movecurs(cy,cx);
                    }
                    break;
                case 'F':
                    ;
                    temp = getchar();
                    found = 0;
                    temp2= cx;
                    temp3 = mBuf.xpos;
                    update_made = 1;

                    smart_moveleft(&mBuf,cx);
                    get_cursor_pos(&cy,&cx);

                    while((int)(mBuf.xpos)+(int)cx-(int)(mBuf.xoffset) > -1){
                        if(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                            found = 1;
                            break;
                        }
                        smart_moveleft(&mBuf,cx);
                        get_cursor_pos(&cy,&cx);
                        if(mBuf.xpos+cx-mBuf.xoffset == 0) break;
                    }
                    if(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                        found = 1;
                    }
                    if(!found){
                        cx= temp2; 
                        mBuf.xpos = temp3;
                        movecurs(cy,cx);
                    }
                    break;
                case 'c':
                    ; 
                    centeronline(&mBuf,cy,cx,WINHEIGHT);
                    update_made = 1;
                    break;
                case 19: //CTRL + S
                    if(mBuf.filename != NULL){
                        int newbytes = calcsize(&mBuf);
                        int oldbytes = calcsize(lastSave);
                        lastsavesize= abs(newbytes-oldbytes);
                        lastSave = saveFile(&mBuf);
                        get24Htime(lastsavetimestr,64);
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,abs(newbytes-oldbytes),lastsavetimestr);
                        hasSaved = 1;
                    }else{
                        strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed File, try /saveas",colors[5]);
                    }
                    break;
                case '/':
                    ;
                    size_t oldRow = cy;
                    size_t oldCol = cx;

                    cmdbuf[cmdi] = ch;
                    update_statusbar(cmdbuf,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                    showcursor();
                    movecurs((size_t)HEIGHT+mBuf.yoffset+1,(size_t)2);
                    cmdi++;
                    char buf[8];
                    int nocheck = 0;
                    while((bytes_read = read(STDIN_FILENO,buf,sizeof(buf))) != -1){
                        if(buf[0] == '\n') break;
                        if(buf[0] == '\033') {nocheck = 1; break;}
                        if(buf[0] == '\t') continue;

                        if((short)buf[0] == 127){
                            cmdbuf[cmdi-1] = '\0';
                            cmdi--;
                            if(cmdi == 0){ nocheck = 1; break; }
                        }else{
                            cmdbuf[cmdi] = buf[0];
                            cmdi++;
                        }
                        update_statusbar(cmdbuf,HEIGHT,WINWIDTH,&mBuf,colors,oldRow,oldCol);
                        movecurs((size_t)WINHEIGHT+STATUSBARHEIGHT,strlen(cmdbuf)+1);
                    }   
                    if(!nocheck){ 
                        trim_trailing_spaces(cmdbuf);

                        if(strcmp(cmdbuf,"/w") == 0){
                            if(mBuf.filename == NULL){
                                strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed file, try /saveas",colors[5]);
                            }else{
                                int newbytes = calcsize(&mBuf);
                                int oldbytes = calcsize(lastSave);
                                lastSave = saveFile(&mBuf);
                                lastsavesize = abs(newbytes-oldbytes);
                        get24Htime(lastsavetimestr,64);
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,abs(newbytes-oldbytes),lastsavetimestr);
                                hasSaved = 1;
                            }
                        } else if(strcmp(cmdbuf,"/wq")== 0){
                            if(mBuf.filename != NULL){
                                int newbytes = calcsize(&mBuf);
                                int oldbytes = calcsize(lastSave);
                                lastSave = saveFile(&mBuf);
                        get24Htime(lastsavetimestr,64);
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,abs(newbytes-oldbytes),lastsavetimestr);
                                quit = 1;
                                break;
                            }else{
                                strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnnamed file, try /saveas",colors[5]);
                            }

                        }else if(strcmp(cmdbuf, "/q")==0){
                            if(hasSaved || mBuf.filename == NULL){
                                quit = 1;
                                break;
                            }else{
                                strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnsaved changes, try /w or /q! to quit without saving",colors[5]);
                            }
                        }else if(strcmp(cmdbuf,"/q!")==0){
                            quit = 1;
                            break;
                        }   
                        else if(strncmp(cmdbuf,"/saveas ",8) == 0){
                            char * newname = cmdbuf+8;
                            int newbytes = calcsize(&mBuf);
                            int oldbytes = calcsize(lastSave);
                        lastsavesize = abs(newbytes-oldbytes);
                            lastSave = saveFile(&mBuf);
                        get24Htime(lastsavetimestr,64);
                        strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,abs(newbytes-oldbytes),lastsavetimestr);
                            hasSaved = 1;
                            char * oldfilename = mBuf.filename;
                            char * new = (char *)malloc(strlen(newname)*sizeof(char));
                            strcpy(new,newname);
                            mBuf.filename = new;
                            if(oldfilename != NULL) free(oldfilename);

                        }else if(strncmp(cmdbuf,"/e! ",4) == 0){
                            char * newname = cmdbuf+4;
                            loadFile(&mBuf,newname);
                            oldRow = mBuf.yoffset;
                            oldCol = mBuf.xoffset;
                            mBuf.ypos = 0;
                            mBuf.xpos = 0;
                            update_made = 1;
                        }else if(strncmp(cmdbuf,"/e ",3) == 0){
                            if(hasSaved){
                                char * newname = cmdbuf+3;
                                strcpy(filename,newname);
                                loadFile(&mBuf,filename);
                                oldRow = mBuf.yoffset;
                                oldCol = mBuf.xoffset;
                                mBuf.ypos = 0;
                                mBuf.xpos = 0;

                            }else{
                                strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnsaved changes, try /w or /e! to edit without saving",colors[5]);
                            }
                            update_made = 1;
                        }else if(strncmp(cmdbuf, "/theme ",7) == 0){
                            char * newtheme = cmdbuf+7; 
                            int changed = 0;
                            for(int i = 0 ; i < zthemecount ; i++) {
                                if(strcmp(newtheme,zthemes[i].name) == 0){
                                    colors = zthemes[i].data;
                                    changed = 1;
                                    update_made = 1;
                                    break;
                                }
                            }
                            if(!changed){
                                strcpyf(statusBarMsg,sblen,"\033[38;5;%dmNo such theme as \'%s\'",colors[5],newtheme);
                            }

                        }else{
                            strcpyf(statusBarMsg,sblen,"\033[38;5;%dmUnknown command \'%s\'",colors[5],cmdbuf);
                        }
                    }
                    memset(cmdbuf,'\0',sizeof(cmdbuf));
                    cmdi = 0;
                    cy = oldRow;
                    cx = oldCol;
                    movecurs(cy,cx);
                    hidecursor();
                    break;
                default:
                    ;
            }
        }
        if(quit)break;

        if (ch == '\033'){
            if(input_buffer[1] == '\0'){
               
                mode = 'n';
                strcpy(modestr,"--NORMAL--");
                strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
                update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
                continue;

                if(mBuf.xpos+cx-mBuf.xoffset == strlen(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset])){
                    smart_moveleft(&mBuf,cx);
                }else; 

            }else if(input_buffer[1] == '['){
            movement:
                if(cursorMovement(&mBuf,input_buffer[2],cy,cx,mode,WINHEIGHT, WINWIDTH))
                    update_made = 1;
            }
            goto end_frame;
        }

        if(mode == 'i'){

            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(mBuf.xpos+cx-mBuf.xoffset== 0 && mBuf.ypos+cy-mBuf.yoffset!= 0){ //CASE OF DELETING LINE

                    char * removed_line = remove_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset);
                    smart_moveup(&mBuf,cy);
                    get_cursor_pos(&cy,&cx);

                    snapCursorRight(&mBuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
                    get_cursor_pos(&cy,&cx);


                    if(removed_line[0] != '\0'){
                        char * aboveline = mBuf.contents[mBuf.ypos+cy-mBuf.yoffset]; 
                        size_t alen = strlen(aboveline);
                        size_t rlen = strlen(removed_line);
                        char * newabove= (char *)realloc(aboveline,(alen+rlen+1)*sizeof(char));
                        aboveline = newabove;
                        mBuf.contents[mBuf.ypos+cy-mBuf.yoffset] = aboveline;
                        strcat(aboveline,removed_line);
                        free(removed_line);
                    }else ;
                }else{
                    if(mBuf.xpos+cx-mBuf.xoffset== 0 && mBuf.ypos+cy-mBuf.yoffset == 0) continue;
                    size_t row = mBuf.ypos+cy-mBuf.yoffset;
                    int col = mBuf.xpos+cx-mBuf.xoffset;
                    if(mBuf.contents[row][col-1] == '\t'){

                        remove_from_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset,col-1);
                        smart_moveleft(&mBuf,cx);
                        get_cursor_pos(&cy, &cx);
                        while(mBuf.xpos+cx-mBuf.xoffset > 0 && mBuf.contents[mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset-1] != '\t' && !isalnum(mBuf.contents[mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset-1])){
                            remove_from_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset-1);
                            smart_moveleft(&mBuf,cx);
                            get_cursor_pos(&cy, &cx);
                        }
                    }else{
                        remove_from_line(&mBuf,row,col-1);
                        smart_moveleft(&mBuf,cx);
                    }
                }
            }else if(ascii_ch == 10){ // '\n'


                size_t copy_start = mBuf.xpos+cx-mBuf.xoffset;
                size_t line_index = mBuf.ypos+cy-mBuf.yoffset;
                char * line = mBuf.contents[line_index];
                size_t line_len = strlen(line);
                if(line_len > 0){

                    //Copy the section of line
                    char * copied_line = line+copy_start;
                    char newl[strlen(copied_line)+1];
                    strcpy(newl,copied_line);
                    newl[strlen(copied_line)+1] = '\0';

                    //Reduce size of old line
                    char * shortened_line = (char *)realloc(line,(copy_start)*sizeof(char)); 
                    if(shortened_line != NULL){
                        strcpy(shortened_line,line);
                        shortened_line[copy_start] = '\0';
                    }else{
                        shortened_line = (char *)malloc(1*sizeof(char));
                        shortened_line[0] = '\0';
                    }

                    mBuf.contents[line_index] = shortened_line;
                    //Insert line
                    insert_line(&mBuf,newl,line_index+1);
                }else{
                    char * line = "\0";
                    insert_line(&mBuf,line,line_index+1);
                }
                smart_movedown(&mBuf,cy,WINHEIGHT);
                get_cursor_pos(&cy,&cx);
                movecurs(cy,(size_t)mBuf.xoffset);
            }else if(ch == '\t'){ //\t

                size_t row = mBuf.ypos+cy-mBuf.yoffset;
                size_t col = mBuf.xpos+cx-mBuf.xoffset;

                for(int i = 0 ; i < tabwidth-1 ; i++){

                    insert_to_line(&mBuf,row,col,' ');
                    smart_moveright_i(&mBuf,cx, strlen(mBuf.contents[row]),WINWIDTH); 
                }
                insert_to_line(&mBuf,row,col+tabwidth-1,'\t');
                smart_moveright_i(&mBuf,cx, strlen(mBuf.contents[row]),WINWIDTH); 


            }else{

                insert_to_line(&mBuf,mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset,ch);
                smart_moveright_i(&mBuf,cx, strlen(mBuf.contents[cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH); 

            }
            update_made = 1;
            hasSaved = 0;
        }

    end_frame:
        get_cursor_pos(&cy,&cx);

        if(update_made){ 
            drawbuffer(mBuf.ycorner,mBuf.xcorner,WINHEIGHT,WINWIDTH,&mBuf,colors);
            movecurs(cy,cx);
            update_made = 0;
        }

        if(statusBarMsg[0] == '\0') 
            strcpyf(statusBarMsg,sblen,"%s  %dB written at %s",modestr,lastsavesize,lastsavetimestr);
        
        update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,&mBuf,colors,cy,cx);
        strcpy(statusBarMsg,"");
        memset(input_buffer,'\0',sizeof(input_buffer));
        snapCursorLeft(&mBuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
        get_cursor_pos(&cy,&cx);
        showcursor();

    }

    for(int i = 0 ; i < mBuf.linecount ; i++){
        free(mBuf.contents[i]);
    }
    if(mBuf.filename != NULL)
        free(mBuf.filename);
    free(modestr);

    resetColor();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    showcursor();
    return 0;
}

