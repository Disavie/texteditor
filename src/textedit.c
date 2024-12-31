#include "ansihelpers.h"
#include "mytui.h"
#include <unistd.h>


int main(int argc, char ** argv){
    openAltBuffer();

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


    size_t linecount;
    size_t longestline;
    size_t szHelper;
    int hasSaved = 1;
    char ** f_buf;
    char filename[32]; 


    if(argc > 1){
        strcpy(filename,argv[1]);
        loadFile(&f_buf,filename,&linecount,&longestline,&szHelper,HEIGHT);

    }else{
        strcpy(filename,"");
        createFile(&f_buf,filename,&linecount,&longestline,&szHelper,HEIGHT);
    }

    //if file is shorter than the terminal height then fill with blank lines
    int i = linecount;
    if(linecount < HEIGHT){
        while(i < szHelper){
            f_buf[i] = (char *)malloc(sizeof(char));
            f_buf[i][0] = '\0';
            i++;
        }
    }

    if(szHelper < HEIGHT){
        szHelper = HEIGHT;
    }

    int yStart = 0;
    int xStart = 0;
    int yOffset = 2 ;
    int xOffset = 7 ;
    short statusBarHeight = 1;
    char statusBarMsg[128] = "";
    short rend_HEIGHT = HEIGHT - yOffset- statusBarHeight;
    short rend_WIDTH = WIDTH - xOffset;
    int cRow = yOffset;
    int cCol = xOffset;
    int hasQuit = 0;
    short update_made = 1;
    char ch = '\0';
    char mode = 'n'; 

    short text_color = 195;
    short bg_file_color = 236;
    short bg_unused_color = 235; 
    short comment_color = 242;
    short border_color = text_color; 
    short error_color = 161;
    short colors[] = {text_color,bg_file_color,bg_unused_color,comment_color,border_color,error_color};

    char temp[128];
    char * modes[32] = {"--NORMAL--", "--INSERT--"};
    strcpy(temp," ");strcat(temp,modes[0]);strcat(temp,"    ");strcat(temp,filename);


    create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,xStart,linecount,colors);
    drawStatusBar(temp,WIDTH,colors,0);
    if(argc == 1){movecurs((int)(cRow+linecount),cCol); drawLogo(rend_HEIGHT,rend_WIDTH, colors); }
    movecurs(cRow,cCol);

    char input_buffer[8];
    memset(input_buffer,'\0',sizeof(input_buffer));
    char motion_buffer[8];
    char prefixes[] = {'d','y'};
    char movements[] = {'w','b','f','$','^'};
    //motion anatomy prefix (d y)
    //count
    //motion type (w f<char> $ ^)
    
    ssize_t bytes_read;
    char cmdbuf[128];
    memset(cmdbuf,'\0',sizeof(cmdbuf));
    short cmdi = 0;
    short isError = 0;

    int quit = 0;

    while((bytes_read = read(STDIN_FILENO,input_buffer,sizeof(input_buffer))) != -1){

        hidecursor();
        isError = 0;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col;
        HEIGHT = w.ws_row;
        rend_HEIGHT = HEIGHT - yOffset-statusBarHeight;
        rend_WIDTH = WIDTH - xOffset ;
        

        if(bytes_read == 0) continue;
        ch = input_buffer[0];

        if(updateMode(ch,&mode)){ 
          update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
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
                    if(linecount == 1) {
                        char * newline = (char *)malloc(sizeof(char));
                        newline[0] = '\0';
                        void * oldline = f_buf[0];
                        f_buf[0] = newline;
                        free(oldline);
                    }else{
                        char * removed_line = remove_line(&f_buf,yStart+cRow-yOffset,&linecount);
                        if(yStart+cRow-yOffset == linecount) smart_moveup(cRow,&yStart,yOffset);
                    }
                    update_made = 1;
                    break;
                case 'w':
                    if(mimic_vim_w(&f_buf, &yStart, &xStart,&cRow, &cCol, yOffset, xOffset,rend_WIDTH))
                        update_made = 1;
                    strcpy(motion_buffer,"");
                    break;
                case 'b':
                    if(mimic_vim_b(&f_buf, &yStart, &xStart,&cRow, &cCol, yOffset, xOffset,rend_WIDTH))
                        update_made = 1;
                    break;
                case 'a':
                    smart_moveright2_insert(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                    get_cursor_pos(&cRow,&cCol);
                    updateMode('i',&mode); 
                    update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                    goto end_frame;
                case 'x':
                    remove_from_line(f_buf,f_buf[yStart+cRow-yOffset],yStart+cRow-yOffset,xStart+cCol-xOffset); 
                    update_made = 1;
                    break;
                case 's':
                    remove_from_line(f_buf,f_buf[yStart+cRow-yOffset],yStart+cRow-yOffset,xStart+cCol-xOffset); 
                    update_made = 1;
                    updateMode('i',&mode); 
                    update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                    goto end_frame;
                case 'o':
                    ;    
                    insert_line(&f_buf,"",yStart+cRow-yOffset+1,&linecount);
                    smart_movedown(cRow,&yStart,1,linecount,rend_HEIGHT);
                    get_cursor_pos(&cRow,&cCol);
                    movecurs(cRow,xOffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode); 
                    update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                    goto end_frame;
                case 'O':
                    ;    
                    insert_line(&f_buf,"",yStart+cRow-yOffset,&linecount);
                    get_cursor_pos(&cRow,&cCol);
                    movecurs(cRow,xOffset);
                    update_made = 1;
                    hasSaved = 0;
                    updateMode('i',&mode); 
                    update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                    goto end_frame;
                case 'f':
                    ;
                    char temp = getchar();
                    short found = 0;
                    size_t temp2= cCol;
                    size_t temp3= xStart;
                    update_made = 1;

                    smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                    get_cursor_pos(&cRow,&cCol);
                    while(xStart+cCol-xOffset < strlen(f_buf[yStart+cRow-yOffset])){
                        if(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] == temp){
                            found = 1;
                            break;
                        }
                        smart_moveright2_insert(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                        get_cursor_pos(&cRow,&cCol);
                    }
                    if(!found){
                        xStart = temp3;
                        cCol= temp2; 
                        movecurs(cRow,cCol);
                    }
                    break;
                case 'F':
                    ;
                    temp = getchar();
                    found = 0;
                    temp2= cCol;
                    temp3 = xStart;
                    update_made = 1;

                    smart_moveleft(cCol,&xStart,xOffset);
                    get_cursor_pos(&cRow,&cCol);
                    while(xStart+cCol-xOffset > -1){
                        if(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] == temp){
                            found = 1;
                            break;
                        }
                        smart_moveleft(cCol,&xStart,xOffset);
                        get_cursor_pos(&cRow,&cCol);
                        if(xStart+cCol-xOffset == 0) break;
                    }
                    if(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] == temp){
                        found = 1;
                    }
                    if(!found){
                        cCol= temp2; 
                        xStart = temp3;
                        movecurs(cRow,cCol);
                    }
                    break;
                case 21:

                    //logLine("you pressed CTRL+U");

                    break;
                case ':':
                    ;
                    size_t oldRow = cRow;
                    size_t oldCol = cCol;

                    cmdbuf[cmdi] = ch;
                    update_statusbad(cmdbuf,WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                    showcursor();
                    movecurs(rend_HEIGHT+yOffset+1,2);
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
                        update_statusbad(cmdbuf,WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                        movecurs(rend_HEIGHT+yOffset+1,(int)strlen(cmdbuf)+1);
                    }   
                    if(!nocheck){ 
                        size_t cmd_length = strlen(cmdbuf);
                        trim_trailing_spaces(cmdbuf);

                        if(strcmp(cmdbuf,":w") == 0){
                            if(strcmp(filename,"[Unnamed File]") == 0){
                                strcpy(statusBarMsg,"Unnamed file, try :saveas");
                                isError = 1;
                            }else{
                                saveFile(filename,linecount,&f_buf);
                                hasSaved = 1;
                            }
                        } else if(strcmp(cmdbuf,":wq") == 0){
                            if(strcmp(filename,"[Unnamed File]") != 0){
                                saveFile(filename,linecount,&f_buf);
                                quit = 1;
                                break;
                            }else{
                                strcpy(statusBarMsg,"Unnamed file, try :saveas");
                                isError = 1;
                            }

                        }else if(strcmp(cmdbuf, ":q")==0){
                            if(hasSaved || strcmp(filename, "[Unnamed File]") == 0){
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
                            saveFile(filename,linecount,&f_buf);
                            hasSaved = 1;
                            strcpy(filename,newname); 
                        }else if(strncmp(cmdbuf,":e!",3) == 0){
                            char * newname = cmdbuf+4;
                            strcpy(filename,newname);
                            loadFile(&f_buf,filename,&linecount,&longestline,&szHelper,HEIGHT);
                            update_made = 1;
                        }else if(strncmp(cmdbuf,":e",2) == 0){
                            if(hasSaved){
                                char * newname = cmdbuf+3;
                                strcpy(filename,newname);
                                loadFile(&f_buf,filename,&linecount,&longestline,&szHelper,HEIGHT);
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
                    cRow = oldRow;
                    cCol = oldCol;
                    movecurs(cRow,cCol);
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
                update_statusbad("",WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
                if(xStart+cCol-xOffset == strlen(f_buf[yStart+cRow-yOffset])){
                    smart_moveleft(cCol,&xStart,xOffset);
                }else;

            }else if(input_buffer[1] == '['){
            movement:
                switch (input_buffer[2]){
                    case 'A':
                        if(smart_moveup(cRow,&yStart,yOffset))
                            update_made = 1;
                        break;
                    case 'B':
                        if(smart_movedown(cRow,&yStart,0,linecount,rend_HEIGHT))
                            update_made = 1;
                        break;

                    case 'C':
                        if(mode == 'i') i = smart_moveright2_insert(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                        else i =smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);

                        switch(i){

                            case 0:
                                break;
                            case 2:
                                continue;
                                if(yStart+cRow-yOffset != linecount-1){
                                    smart_movedown(cRow,&yStart,0,linecount,rend_HEIGHT);
                                    get_cursor_pos(&cRow,&cCol);
                                    create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,(xStart=0),linecount,colors);
                                    movecurs(cRow,xOffset);
                                }
                            default:
                                update_made = 1;
                        }
                        break;

                    case 'D':
                        switch (smart_moveleft(cCol,&xStart,xOffset)){
                            case 0:
                                break;
                            case 2:
                                continue;
                                if(yStart+cRow-yOffset != 0){
                                    smart_moveup(cRow,&yStart,yOffset);
                                    get_cursor_pos(&cRow,&cCol);
                                    snapCursorRight(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount,colors);
                                    smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                                    get_cursor_pos(&cRow,&cCol);
                                }
                            default:
                                update_made = 1;
                        }

                        break;
                    default:
                        ;
                }
            }
        }

        if(mode == 'i'){

            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(xStart+cCol-xOffset == 0 && yStart+cRow-yOffset != 0){ //CASE OF DELETING LINE

                    char * removed_line = remove_line(&f_buf,yStart+cRow-yOffset,&linecount);
                    smart_moveup(cRow,&yStart,yOffset);
                    get_cursor_pos(&cRow,&cCol);

                    snapCursorRight(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount,colors);
                    get_cursor_pos(&cRow,&cCol);


                    if(removed_line[0] != '\0'){
                        char * aboveline = f_buf[yStart+cRow-yOffset]; 
                        size_t alen = strlen(aboveline);
                        size_t rlen = strlen(removed_line);
                        char * newabove= (char *)realloc(aboveline,(alen+rlen+1)*sizeof(char));
                        aboveline = newabove;
                        f_buf[yStart+cRow-yOffset] = aboveline;
                        char * o = strcat(aboveline,removed_line);
                        free(removed_line);
                    }else ;
                }else{
                    if(xStart+cCol-xOffset == 0 && yStart+cRow-yOffset == 0) continue;
                    char * line = f_buf[yStart+cRow-yOffset];
                    char * ret = remove_from_line(f_buf,line,yStart+cRow-yOffset,xStart+cCol-xOffset-1);
                    smart_moveleft(cCol,&xStart,xOffset);
                }
            }else if(ascii_ch == 10){ // '\n'


                size_t copy_start = xStart+cCol-xOffset;
                size_t line_index = yStart+cRow-yOffset;
                char * line = f_buf[line_index];
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

                    f_buf[line_index] = shortened_line;
                    //Insert line
                    char * t = insert_line(&f_buf,newl,line_index+1,&linecount);
                }else{
                    char * line = "\0";
                    char * t = insert_line(&f_buf,line,line_index+1,&linecount);
                }
                smart_movedown(cRow,&yStart,1,linecount,rend_HEIGHT);
                get_cursor_pos(&cRow,&cCol);
                movecurs(cRow,xOffset);
            }else{

                char * line= insert_to_line(f_buf,f_buf[yStart+cRow-yOffset],yStart+cRow-yOffset, xStart+cCol-xOffset, ch);
                longestline = countLongestLineBuffer(f_buf,linecount);
                if(mode == 'i') smart_moveright2_insert(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                else smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);

            }
            update_made = 1;
            hasSaved = 0;
        }
        
        end_frame:
        get_cursor_pos(&cRow,&cCol);
        if(update_made){ 
            longestline = countLongestLineBuffer(f_buf,linecount);
            create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,xStart,linecount,colors);
            movecurs(cRow,cCol);
            update_made = 0;
        }


        update_statusbad(statusBarMsg,WIDTH,rend_HEIGHT,modes,mode,cRow,cCol,yOffset,filename,colors,isError);
        strcpy(statusBarMsg,"");
        memset(input_buffer,'\0',sizeof(input_buffer));
        snapCursorLeft(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount,colors);
        get_cursor_pos(&cRow,&cCol);
        showcursor();

    }

    //restore cursor
    resetColor();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    showcursor();
    return 0;
}




