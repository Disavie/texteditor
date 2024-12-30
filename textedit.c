#include "ansihelpers.h"
#include "mytui.h"
#include <unistd.h>


int main(int argc, char ** argv){
    openAltBuffer();
    clearLog();

    struct termios oldtermios;
    set_input_mode(&oldtermios);
    setbuf(stdout,NULL);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    short WIDTH = w.ws_col;
    short HEIGHT = w.ws_row;

    char ch; 
    size_t linecount;
    size_t longestline;
    size_t szHelper;
    char ** f_buf;
    char filename[32]; //<- length of filename
    FILE * f;
    int i = 0;


    if(argc > 1){
        strcpy(filename,argv[1]);

        f = fopen(filename,"r");
        linecount = countLines(f);
        longestline  = countLongestLine(f);
        fclose(f);

        szHelper = linecount; 
        if(linecount < HEIGHT){
            szHelper = HEIGHT;
        }
        char ** new_f_buf = (char **)malloc(szHelper*sizeof(char *));
        f_buf = new_f_buf;

        f = fopen(argv[1],"r");    
        i = 0;
        while(!feof(f) && i < linecount){
            char line[512];
            fgets(line,sizeof(line),f);
            size_t len = strlen(line);
            f_buf[i] = (char *)malloc((len+1)*sizeof(char));
            strcpy(f_buf[i],line);
            if(f_buf[i][len-1] == '\n')
                f_buf[i][len-1] = '\0';
            logLine("read :");logLine(f_buf[i]);
            i++;
        }
        logLine("read them all");
        fclose(f);

    }else{
        linecount = 1;
        longestline = 0;
        szHelper = HEIGHT;
        char ** new_f_buf = (char **)malloc(szHelper*sizeof(char *));
        f_buf = new_f_buf;
    }

    //if file is shorter than the terminal height then fill with blank lines
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
    int yOffset = 2;
    int xOffset = 7;
    short rend_HEIGHT = HEIGHT - yOffset-1;
    short rend_WIDTH = WIDTH - xOffset;
    int cRow = yOffset;
    int cCol = xOffset;
    int hasQuit = 0;
    short update_made = 1;
    ch = '\0';
    char mode = 'n'; 

    create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,xStart,linecount);
    if(argc == 1){ movecurs((int)(cRow+linecount),cCol); drawLogo(rend_HEIGHT,rend_WIDTH); }
    movecurs(cRow,cCol);

    char input_buffer[8];
    ssize_t bytes_read;

    while((bytes_read = read(STDIN_FILENO,input_buffer,sizeof(input_buffer))) != -1){
        
        hidecursor();
        clearLog();
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col;
        HEIGHT = w.ws_row;
        rend_HEIGHT = HEIGHT - yOffset-1;
        rend_WIDTH = WIDTH - xOffset ;
        if(bytes_read == 0) continue;
        ch = input_buffer[0];
        for(int i = 0 ; i < 8 ; i++){
          logChar(input_buffer[i]);
        }


        if(updateMode(ch,&mode)) continue; 
        
        if(mode == 'n'){
          if(ch == 'h' || ch == 'j' || ch == 'k' || ch == 'l'){
            switch(ch){
              case 'h':
                input_buffer[2] = 'D';
                break;
              case 'j':
                input_buffer[2] = 'B';
                break;
              case 'k':
                input_buffer[2] = 'A';
                break;
              case 'l':
                input_buffer[2] = 'C';
                break;
            }
            goto movement;
          }else if(ch == 'q'){
            break;
          }else if(ch == 'd'){
            if(linecount == 1) continue;
            char * removed_line = remove_line(&f_buf,yStart+cRow-yOffset,&linecount);
            if(yStart+cRow-yOffset == linecount) smart_moveup(cRow,&yStart,yOffset);
            update_made = 1;
          }else if(ch == 'w'){

            while(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] != ' '){
              logLine("\n");
              logNum(cCol);
              logNum(xOffset);
              logNum(rend_WIDTH);
              logLine("\n");
              if(cCol-xOffset == rend_WIDTH-1){
                xStart++;
                update_made = 1;
              }else{
                cCol++;
              }
              
              if(cCol+xStart-xOffset >= strlen(f_buf[yStart+cRow-yOffset])) break;
            }

            movecurs(cRow,cCol);
            smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);

          }else if(ch == 'b'){
            if(xStart+cCol-xOffset == 0)continue;

            if(cCol == xOffset) {xStart--;}
            else {cCol--;}
        
            if(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] == ' '){

              if(cCol == xOffset) {xStart--;}
              else {cCol--;}
              
            }

            int loops = 0;
            while(f_buf[yStart+cRow-yOffset][xStart+cCol-xOffset] != ' '){
              if(cCol == xOffset && xStart != 0){
                xStart--;
                logLine("adjusting screen\n");
                update_made = 1;
              }else{
                logLine("decremening\n");
                cCol--;
              }

              if(cCol+xStart-xOffset == 0) {  break; };
              loops++;
            }
            movecurs(cRow,cCol);
            if(cCol+xStart-xOffset != 0)
              smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
          }
        }

        if (ch == '\033'){
            if(input_buffer[1] == '['){
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
                      switch(smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH)){

                        case 0:
                          break;
                        case 2:
                          if(yStart+cRow-yOffset != linecount-1){
                            smart_movedown(cRow,&yStart,0,linecount,rend_HEIGHT);
                            get_cursor_pos(&cRow,&cCol);
                            create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,(xStart=0),linecount);
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
                              if(yStart+cRow-yOffset != 0){
                                smart_moveup(cRow,&yStart,yOffset);
                                get_cursor_pos(&cRow,&cCol);
                                snapCursorRight(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount);
                                smart_moveright2(cCol, &xStart, xOffset,strlen(f_buf[cRow+yStart-yOffset]),rend_WIDTH);
                                get_cursor_pos(&cRow,&cCol);
                              }
                          default:
                            update_made = 1;
                      }

                      break;
                  default:
                    logLine("idunno what escape sequence this shit is\n");
              }
            }else{
              if(input_buffer[1] == '\0'){ //case of hitting ESC
                mode = 'n'; 
              }else;
            }
            goto end_frame;
        }

        if(mode == 'i'){

            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(xStart+cCol-xOffset == 0 && yStart+cRow-yOffset != 0){ //CASE OF DELETING LINE

                    char * removed_line = remove_line(&f_buf,yStart+cRow-yOffset,&linecount);
                    smart_moveup(cRow,&yStart,yOffset);
                    get_cursor_pos(&cRow,&cCol);

                    snapCursorRight(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount);
                    get_cursor_pos(&cRow,&cCol);


                    if(removed_line[0] != '\0'){
                        char * aboveline = f_buf[yStart+cRow-yOffset]; 
                        size_t alen = strlen(aboveline);
                        size_t rlen = strlen(removed_line);
                        logNum(rlen);
                        char * newabove= (char *)realloc(aboveline,(alen+rlen+1)*sizeof(char));
                        logLine("realloc passed");   
                        aboveline = newabove;
                        f_buf[yStart+cRow-yOffset] = aboveline;
                        char * o = strcat(aboveline,removed_line);
                        free(removed_line);
                    }else ;
                    logLine("done removing");
                }else{
                    if(xStart+cCol-xOffset == 0 && yStart+cRow-yOffset == 0) continue;
                    logLine("_____REMOVEFROMLINE_____");
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
                    logLine(newl);

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
                    logLine("inserting blank new line");
                    char * line = "\0";
                    char * t = insert_line(&f_buf,line,line_index+1,&linecount);
                }
                smart_movedown(cRow,&yStart,1,linecount,rend_HEIGHT);
                get_cursor_pos(&cRow,&cCol);
                create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,(xStart=0),linecount);
                movecurs(cRow,xOffset);
            }else{

                char * line= insert_to_line(f_buf,f_buf[yStart+cRow-yOffset],yStart+cRow-yOffset, xStart+cCol-xOffset, ch);
                longestline = countLongestLineBuffer(f_buf,linecount);
                smart_moveright2(cCol, &xStart, xOffset, strlen(line), rend_WIDTH);

            }
            logLine("update_made = 1");
            update_made = 1;
        }
        
        end_frame:
        get_cursor_pos(&cRow,&cCol);
        if(update_made){ 
            longestline = countLongestLineBuffer(f_buf,linecount);
            create_window_inoutRANGE(0,0,rend_HEIGHT,rend_WIDTH,f_buf,yStart,xStart,linecount);
            movecurs(cRow,cCol);
            update_made = 0;
        }

        memset(input_buffer,'\0',sizeof(input_buffer));
        snapCursorLeft(f_buf,&cRow,&cCol,&yStart,&xStart,yOffset,xOffset,rend_HEIGHT,rend_WIDTH,linecount);
        get_cursor_pos(&cRow,&cCol);
        showcursor();

    }

    //saving the file
    f = fopen(filename,"w");
    if (f == NULL){
        logLine("No filename");
    }else{
        for(int i = 0 ; i < linecount; i++){
            fprintf(f,"%s\n",f_buf[i]);
        }
        fclose(f);
    }
    //restore cursor
    resetColor();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    showcursor();
    return 0;
}
