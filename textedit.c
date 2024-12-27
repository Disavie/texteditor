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

    short WIDTH = w.ws_col - 2;
    short HEIGHT = w.ws_row;

    char ch; 
    FILE * f;
    char filename[strlen(argv[1])];
    strcpy(filename,argv[1]);

    f = fopen(argv[1],"r");
    size_t linecount = countLines(f);
    size_t longestline  = countLongestLine(f);

    printf("longestline in the file %s is %lu letters\n",argv[1],longestline);
    printf("file %s is %lu lines\n",argv[1],linecount);
    fclose(f);

    size_t szHelper = linecount; 
    if(linecount < HEIGHT){
        szHelper = HEIGHT;
    }
    char ** f_buf = (char **)malloc(szHelper*sizeof(char *));

    /*
    printf("got here\n");
    printf("height = %d, width = %d, \n",HEIGHT,WIDTH);
    sleep(2);
    */
    char * ptr_longestline;
    f = fopen(argv[1],"r");    
    int i = 0;
    while(!feof(f) && i < linecount){
        char line[512];
        fgets(line,sizeof(line),f);
        size_t len = strlen(line);
        f_buf[i] = (char *)malloc((len+1)*sizeof(char));
        strcpy(f_buf[i],line);
        if(f_buf[i][len-1] == '\n')
            f_buf[i][len-1] = '\0';
        if(len == longestline)
            ptr_longestline = f_buf[i];
        i++;
    }
    //if file is shorter than the terminal height then fill with blank lines
    if(linecount < HEIGHT){
        while(i < szHelper){
            f_buf[i] = (char *)malloc(sizeof(char));
            f_buf[i][0] = '\0';
            i++;
        }
    }

    fclose(f);
    printf("exited the file successfully\n");


    short rend_HEIGHT = HEIGHT - 3;

    int yStart = 0;
    int xStart = 0;
    int cRow = 2;
    int cCol = 2;
    int hasQuit = 0;
    short update_made = 1;
    ch = '\0';
    /*
    printf("got here\n");
    printf("height = %d, width = %d, yStart = %d, xStart = %d\n",HEIGHT,WIDTH,Ystart,xStart);
    sleep(2);
    */
    create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,yStart,xStart);
    movecurs(cRow,cCol);

    char input_buffer[8];
    ssize_t bytes_read;

    while((bytes_read = read(STDIN_FILENO,input_buffer,sizeof(input_buffer))) != -1){
        

        //logLine("-----------New loop");
             

        //logLine("\ncRow = "); logNum(cRow);
        //logLine("\ncCol = "); logNum(cCol);

        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col - 2;
        HEIGHT = w.ws_row;
        rend_HEIGHT = HEIGHT - 3;
        if(bytes_read == 0) continue;
        ch = input_buffer[0];
        /*
        for(int i = 0 ; i < 8 ; i++){
            logChar(input_buffer[i]);
        }
        */



        if (ch == 'q'){
            break;
        }else if (ch == '\033' && input_buffer[1] == '['){
            switch (input_buffer[2]){
                case 'A':
                    if(smart_moveup(cRow,&yStart))
                        update_made = 1;
                    break;
                case 'B':
                    if(smart_movedown(cRow,&yStart,linecount,rend_HEIGHT))
                        update_made = 1;
                    break;
                case 'C':
                    if(smart_moveright(cCol,&xStart,longestline,WIDTH))
                        update_made = 1;
                    break;
                case 'D':
                    if(smart_moveleft(cCol,&xStart))
                        update_made = 1;
                    break;
                default:
                    printf("unknown escape sequence: \\033[%c\n",ch);
            }
        }else{
         //   logLine("I am in the default else");
            short ascii_ch = (short)ch;
          //  logLine("ASCII converted");
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(cCol == 2 && xStart == 0){ //CASE OF DELETING BLANK LINE


                }else{

                    char * line = remove_from_line(f_buf,f_buf[yStart+cRow-2],yStart+cRow-2,xStart+cCol-3);
                    smart_moveleft(cCol,&xStart);
                }
            }else if(ascii_ch == 10){ // '\n'
                movedown();
                //DO OTHER STUFF TO ALLOCATE NEWLIEN TO FULL_BUFFER
            }else{
           //     logLine("\nyStart =");logNum(yStart);
            //    logLine("\ncRow = ");logNum(cRow);
             //   logLine("\nyStart +cRow -2 = "); logNum(yStart+cRow-2);

                char * line= insert_to_line(f_buf,f_buf[yStart+cRow-2],yStart+cRow-2, xStart+cCol-2, ch);
              //  logLine("Inserted!");
                smart_moveright(cCol,&xStart,longestline,WIDTH);

                get_cursor_pos(&cRow,&cCol);

            }
            update_made = 1;

        }

        
        get_cursor_pos(&cRow,&cCol);

       // logLine("\ncRow = "); logNum(cRow);
       // logLine("\ncCol = "); logNum(cCol);

        if(update_made){ 
            // this does work but its chatgpt generated
            //noflicker_create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,yStart,xStart);
            longestline = countLongestLineBuffer(f_buf,szHelper);
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,yStart,xStart);
            movecurs(cRow,cCol);
            update_made = 0;
        }
        memset(input_buffer,'\0',sizeof(input_buffer));
        //logLine("reset input_buffer");

        //logLine("\ncRow = "); logNum(cRow);
        //logLine("\ncCol = "); logNum(cCol);

        snap_left(f_buf, &cRow,&cCol, yStart,xStart,2,2);

        get_cursor_pos(&cRow,&cCol);
        //logLine("end of loop");

    }

    if(bytes_read == -1){
        logLine("error from read()");
        perror("read");
   }


    //saving the file
    f = fopen(argv[1],"w");
    if (f == NULL) printf("error opening file: %s\n",argv[1]);
    for(int i = 0 ; i < linecount ; i++){
        fprintf(f,"%s\n",f_buf[i]);
    }
    fclose(f);
    //restore cursor
    resetColor();
    clear_screen();
    move00();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    return 0;
}
