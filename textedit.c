#include "ansihelpers.h"
#include "mytui.h"


int main(int argc, char ** argv){

    struct termios oldtermios;
    set_input_mode(&oldtermios);

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    short WIDTH = w.ws_col - 2;
    short HEIGHT = w.ws_row;

    char ch; 
    FILE * f;
    char filename[strlen(argv[1])];
    strcpy(filename,argv[1]);

    f = fopen(argv[1],"r");
    int linecount = countlines(f);
    fclose(f);

    int szHelper = linecount; 
    if(linecount < HEIGHT){
        szHelper = HEIGHT;
    }
    char ** f_buf = (char **)malloc(szHelper*sizeof(char *));

    /*
    printf("got here\n");
    printf("height = %d, width = %d, \n",HEIGHT,WIDTH);
    sleep(2);
    */
    f = fopen(argv[1],"r");    
    int i = 0;
    while(!feof(f) && i < linecount){
        char line[256];

        fgets(line,sizeof(line),f);
        size_t len = strlen(line);
        char * shortened_line = (char *)malloc(len*sizeof(char));
        f_buf[i] = shortened_line;
        strcpy(f_buf[i],line);
        if(f_buf[i][len-1] == '\n')
            f_buf[i][len-1]= '\0';
        printf("read line : %s \n" ,f_buf[i]);
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
    /*
    printf("got here\n");
    printf("height = %d, width = %d, yStart = %d, xStart = %d\n",HEIGHT,WIDTH,Ystart,xStart);
    sleep(2);
    */

    //alternate buffer to reduce flickering
    openAltBuffer();
    draw_borders(0,0,rend_HEIGHT,WIDTH);
    closeAltBuffer();


    while(!hasQuit){



        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        WIDTH = w.ws_col - 2;
        HEIGHT = w.ws_row;
        rend_HEIGHT = HEIGHT - 3;
        if(update_made){ 
            // this does work but its chatgpt generated
            //noflicker_create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,yStart,xStart);
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,yStart,xStart);
            movecurs(cRow,cCol);
            update_made = 0;
        }
        
        snap_left(f_buf, &cRow,&cCol, yStart,xStart,2,2);
        ch = getchar();
        flush_stdin();

        if (ch == 'q'){
            hasQuit = 1;
        }else if (ch == '\033'){
            ch = getchar(); //skips the [ character
            if (ch == '['){

                ch = getchar(); //A/B/C/D
                switch (ch){
                    case 'A':
                        if(cRow > 2){
                             moveup(); 
                        }else{
                            if(yStart!= 0){
                                yStart-=1;
                                update_made = 1;
                            }
                        }
                        break;
                    case 'B':
                        if(yStart+cRow <= linecount){
                            /*
                            printf("yStart+cRow = %d linecount = %d\n",Ystart+cRow,linecount);
                            sleep(1);
                            */
                            if(cRow == rend_HEIGHT && yStart+cRow != linecount){ 
                                yStart+=1;   
                                update_made = 1;
                            }else{
                                movedown();
                            }
                        }
                        break;
                    case 'C':
                        if(cCol <= WIDTH) moveright();
                        break;
                    case 'D':
                        if(cCol > 2) moveleft();
                        break;
                    default:
                        printf("unknown escape sequence: \\033[%c\n",ch);
                        flush_stdin();
                        continue;
                }
             }else{
                printf("unknown escape sequence: \\033%c\n",ch);
                printf("please work");
                    
            }
        }else{
            
            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(cCol >= 2){
                    f_buf[yStart+cRow-2][xStart+cCol-2] = '\0';
                    movecurs(cRow,cCol);
                    printf(" ");
                    if(cCol > 2){
                        moveleft();
                        moveleft();
                    }else;
                }else
                    continue;
            }else if(ascii_ch == 10){ // '\n'
                movedown();
                //DO OTHER STUFF TO ALLOCATE NEWLIEN TO FULL_BUFFER
            }else{
                f_buf[yStart+cRow-2][xStart+cCol-2] = ch;
                movecurs(cRow,cCol);
                printf("%c",ch);
            }
            update_made = 1;
        }

        get_cursor_pos(&cRow,&cCol);
        flush_stdin();
    }
    //saving the file
    f = fopen(argv[1],"w");
    if (f == NULL) printf("error opening file: %s\n",argv[1]);
    for(int i = 0 ; i < linecount ; i++){
        fprintf(f,"%s\n",f_buf[i]);
    }
    fclose(f);
    //restore cursor
    clear_screen();
    move00();
    restore_input_mode(&oldtermios);
    return 0;
}


