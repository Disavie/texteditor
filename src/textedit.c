#include <unistd.h>
#include "mytui.h"
#include "Buffer.h"
#include "cursor.h"
#include "ansihelpers.h"
#include "colorscheme.h"

int main(int argc, char ** argv){
    openAltBuffer();
   // clearLog();

    struct termios oldtermios;
    set_input_mode(&oldtermios);
    setbuf(stdout,NULL);
    signal(SIGINT,handle_signal);
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
        .yoffset = 0,
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
    size_t cy = mBuf.yoffset+1;
    size_t cx = mBuf.xoffset;
    short update_made = 1;
    char ch = '\0';
    char mode = 'n'; 

    const short * colors = gptcolors2;

    char temp[128];
    char * modes[32] = {"--NORMAL--", "--INSERT--"};
    
    short isError = 0;
    short WINHEIGHT = HEIGHT - STATUSBARHEIGHT;
    short WINWIDTH = WIDTH;

    drawbuffer(mBuf.yoffset,mBuf.xoffset,WINHEIGHT,WINWIDTH,&mBuf,colors);
    movecurs(cy,cx);
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
    if(argc == 1){movecurs((mBuf.linecount),(size_t)0); drawLogo(HEIGHT,WIDTH, colors); }
    movecurs(cy,cx);
    showcursor();

    char input_buffer[8];
    memset(input_buffer,'\0',sizeof(input_buffer));
    //motion anatomy prefix (d y)
    //count
    //motion type (w f<char> $ ^)
    
    ssize_t bytes_read;
    char cmdbuf[128];
    memset(cmdbuf,'\0',sizeof(cmdbuf));
    short cmdi = 0;

    int quit = 0;

    while((bytes_read = read(STDIN_FILENO,input_buffer,sizeof(input_buffer))) != -1){

        hidecursor();
        isError = 0;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col;
        HEIGHT = w.ws_row;
        WINHEIGHT = HEIGHT - STATUSBARHEIGHT;
        WINWIDTH = WIDTH; 

        if(bytes_read == 0) continue;
        ch = input_buffer[0];

        if(updateMode(ch,&mode)){ 
            update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
          continue;
        }
        
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
                case 'd':
                    if(mBuf.linecount == 1) {
                        char * newline = (char *)malloc(sizeof(char));
                        newline[0] = '\0';
                        void * oldline = mBuf.contents[0];
                        mBuf.contents[0] = newline;
                        free(oldline);
                    }else{
                        remove_line(&mBuf,-1+mBuf.ypos+cy+mBuf.yoffset);
                        if(-1+mBuf.ypos+cy-mBuf.yoffset == mBuf.linecount) smart_moveup(&mBuf,cx);
                    }
                    update_made = 1;
                    break;
                case 'w':
                    if(mimic_vim_w(&mBuf,cy-1,&cx,WINWIDTH))
                        update_made = 1;
                    break;
                case 'b':
                    if(mimic_vim_b(&mBuf,cy-1,&cx,WINWIDTH))
                        update_made = 1;
                    break;
                case 'a':
                    smart_moveright_i(&mBuf,cx,strlen(mBuf.contents[-1+cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
                    get_cursor_pos(&cy,&cx);
                    updateMode('i',&mode); 
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
                    goto end_frame;
                case 'x':
                    remove_from_line(&mBuf,-1+mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset);
                    update_made = 1;
                    break;
                case 's':
                    remove_from_line(&mBuf,-1+mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset);
                    update_made = 1;
                    updateMode('i',&mode); 
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
                    goto end_frame;
                case 'o':
                    ;    
                    insert_line(&mBuf,"",mBuf.ypos+cy-mBuf.yoffset);
                    smart_movedown(&mBuf,cy,0,WINHEIGHT);
                    get_cursor_pos(&cy,&cx);
                    movecurs(cy,(size_t)mBuf.xoffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode); 
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
                    goto end_frame;
                case 'O':
                    ;    
                    insert_line(&mBuf,"",-1+mBuf.ypos+cy-mBuf.yoffset);
                    get_cursor_pos(&cy,&cx);
                    movecurs(cy,(size_t)mBuf.xoffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode); 
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
                    goto end_frame;
                case 'f':
                    ;
                    char temp = getchar();
                    short found = 0;
                    size_t temp2= cx;
                    size_t temp3= mBuf.xpos;
                    update_made = 1;

                    smart_moveright_n(&mBuf,cx,strlen(mBuf.contents[-1+cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
                    get_cursor_pos(&cy,&cx);
                    while(mBuf.xpos+cx-mBuf.xoffset < strlen(mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset])){
                        if(mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                            found = 1;
                            break;
                        }
                        smart_moveright_n(&mBuf,cx,strlen(mBuf.contents[-1+cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH);
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

                    if(!smart_moveleft(&mBuf,cx))cx--;

                    while((int)mBuf.xpos+(int)cx-(int)mBuf.xoffset > -1){
                        if(mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                            found = 1;
                            break;
                        }
                        if(!smart_moveleft(&mBuf,cx))cx--;
                        if(mBuf.xpos+cx-mBuf.xoffset == 0) break;
                    }
                    if(mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset][mBuf.xpos+cx-mBuf.xoffset] == temp){
                        found = 1;
                    }
                    if(!found){
                        cx= temp2; 
                        mBuf.xpos = temp3;
                        movecurs(cy,cx);
                    }
                    break;
                case 21:

                    //logLine("you pressed CTRL+U");

                    break;
                case ':':
                    ;
                    size_t oldRow = cy;
                    size_t oldCol = cx;

                    cmdbuf[cmdi] = ch;
    update_statusbar(cmdbuf,HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
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
                            if(cmdi == 0) break;
                        }else{
                            cmdbuf[cmdi] = buf[0];
                            cmdi++;
                        }
                        update_statusbar(cmdbuf,HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,oldRow,oldCol);
                        movecurs((size_t)WINHEIGHT+STATUSBARHEIGHT,strlen(cmdbuf)+1);
                    }   
                    if(!nocheck){ 
                        trim_trailing_spaces(cmdbuf);

                        if(strcmp(cmdbuf,":w") == 0){
                            if(mBuf.filename == NULL){
                                strcpy(statusBarMsg,"Unnamed file, try :saveas");
                                isError = 1;
                            }else{
                                saveFile(&mBuf);
                                hasSaved = 1;
                            }
                        } else if(strcmp(cmdbuf,":wq")== 0){
                            if(mBuf.filename != NULL){
                                saveFile(&mBuf);
                                quit = 1;
                                break;
                            }else{
                                strcpy(statusBarMsg,"Unnamed file, try :saveas");
                                isError = 1;
                            }

                        }else if(strcmp(cmdbuf, ":q")==0){
                            if(hasSaved || mBuf.filename == NULL){
                                quit = 1;
                                break;
                            }else{
                                strcpy(statusBarMsg,"Unsaved changes, try :w or :q! to quit without saving");
                                isError = 1;
                            }
                        }else if(strcmp(cmdbuf,":q!")==0){
                            quit = 1;
                            break;
                        }   
                        else if(strncmp(cmdbuf,":saveas ",7) == 0){
                            char * newname = cmdbuf+8;
                            saveFile(&mBuf);
                            hasSaved = 1;
                            char * oldfilename = mBuf.filename;
                            char * new = (char *)malloc(strlen(newname)*sizeof(char));
                            strcpy(new,newname);
                            mBuf.filename = new;
                            if(oldfilename != NULL) free(oldfilename);

                        }else if(strncmp(cmdbuf,":e!",3) == 0){
                            char * newname = cmdbuf+4;
                            loadFile(&mBuf,newname);
                            oldRow = 1;
                            oldCol = mBuf.xoffset;
                            update_made = 1;
                        }else if(strncmp(cmdbuf,":e",2) == 0){
                            if(hasSaved){
                                char * newname = cmdbuf+3;
                                strcpy(filename,newname);
                                loadFile(&mBuf,filename);
                                oldRow = 1;
                                oldCol = mBuf.xoffset;

                            }else{
                                strcpy(statusBarMsg,"Unsaved changes, try :w or :e! to edit without saving");
                                isError=1;
                            }
                            update_made = 1;
                        }else{
                            strcpy(statusBarMsg,"Unknown command \'"); 
                            strcat(statusBarMsg,cmdbuf);
                            strcat(statusBarMsg,"\'");
                            isError = 1;
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
    update_statusbar("",HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);

                if(mBuf.xpos+cx-mBuf.xoffset == strlen(mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset])){
                    smart_moveleft(&mBuf,cx);
                }else; 

            }else if(input_buffer[1] == '['){
            movement:
                if(cursorMovement(&mBuf,input_buffer[2],cy,cx,mode,WINHEIGHT, WINWIDTH))
                   update_made = 1;
            }
        }

        if(mode == 'i'){

            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(mBuf.xpos+cx-mBuf.xoffset== 0 && -1+mBuf.ypos+cy-mBuf.yoffset!= 0){ //CASE OF DELETING LINE

                    char * removed_line = remove_line(&mBuf,-1+mBuf.ypos+cy);
                    smart_moveup(&mBuf,cy);
                    get_cursor_pos(&cy,&cx);

                    snapCursorRight(&mBuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
                    get_cursor_pos(&cy,&cx);


                    if(removed_line[0] != '\0'){
                        char * aboveline = mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset]; 
                        size_t alen = strlen(aboveline);
                        size_t rlen = strlen(removed_line);
                        char * newabove= (char *)realloc(aboveline,(alen+rlen+1)*sizeof(char));
                        aboveline = newabove;
                        mBuf.contents[-1+mBuf.ypos+cy-mBuf.yoffset] = aboveline;
                        strcat(aboveline,removed_line);
                        free(removed_line);
                    }else ;
                }else{
                    if(mBuf.xpos+cx-mBuf.xoffset== 0 && -1+mBuf.ypos+cy-mBuf.yoffset == 0) continue;
                    remove_from_line(&mBuf,-1+mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset-1);
                    smart_moveleft(&mBuf,cx);
                }
            }else if(ascii_ch == 10){ // '\n'


                size_t copy_start = mBuf.xpos+cx-mBuf.xoffset;
                size_t line_index = -1+mBuf.ypos+cy-mBuf.yoffset;
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
                smart_movedown(&mBuf,cy,1,WINHEIGHT);
                get_cursor_pos(&cy,&cx);
                movecurs(cy,(size_t)mBuf.xoffset);
            }else{

                insert_to_line(&mBuf,-1+mBuf.ypos+cy-mBuf.yoffset,mBuf.xpos+cx-mBuf.xoffset,ch);
                if(mode == 'i') smart_moveright_i(&mBuf,cx, strlen(mBuf.contents[-1+cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH); 
                else smart_moveright_n(&mBuf,cx, strlen(mBuf.contents[-1+cy+mBuf.ypos-mBuf.yoffset]),WINWIDTH); 

            }
            update_made = 1;
            hasSaved = 0;
        }
        
        end_frame:
        get_cursor_pos(&cy,&cx);

        if(update_made){ 
            drawbuffer(0,0,WINHEIGHT,WINWIDTH,&mBuf,colors);
            movecurs(cy,cx);
            update_made = 0;
        }


    update_statusbar(statusBarMsg,HEIGHT,WINWIDTH,modes,mode,&mBuf,colors,isError,cy,cx);
        strcpy(statusBarMsg,"");
        memset(input_buffer,'\0',sizeof(input_buffer));
        snapCursorLeft(&mBuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
        get_cursor_pos(&cy,&cx);
        showcursor();

    }

    //restore cursor
    resetColor();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    showcursor();
    return 0;
}




