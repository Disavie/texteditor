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

    for(int i = 0 ; i < szHelper ; i++){
        f_buf[i] = (char *)malloc((WIDTH+1)*sizeof(char));
        memset(f_buf[i], '\0',WIDTH+1);
    }

    f = fopen(argv[1],"r");    
    int i = 0;
    while(!feof(f)){
        fgets(f_buf[i],WIDTH,f);
        size_t len = strlen(f_buf[i]);
        if(f_buf[i][len-1] == '\n')
            f_buf[i][len-1]= '\0';

        i++;
    }
    fclose(f);


    short rend_HEIGHT = HEIGHT - 3;

    int start = 0;
    int end = linecount;
    if(linecount < rend_HEIGHT) end = rend_HEIGHT; 


    int cRow = 2;
    int cCol = 2;
    int hasQuit = 0;
    int change_made = 1;
    while(!hasQuit){

        
        if(change_made){
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,f_buf,start,end);
            printf("\033[%d;%dH",cRow,cCol);
            change_made = 0;
        }

        snap_left(f_buf, &cRow,&cCol, start-2,-2);
        ch = getchar();
        flush_stdin();

        if(ch == '\0') continue; // no changes were made  
        if (ch == 'q'){
            hasQuit = 1;
        }else if (ch == '\033'){
            ch = getchar(); //skips the [ character
            if (ch == '['){

                ch = getchar(); //A/B/C/D
                switch (ch){
                    case 'A':
                        if(cRow > 2)moveup(); 
                        break;
                    case 'B':
                        if(cRow <= linecount) movedown();
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
                    f_buf[start+cRow-2][cCol-2] = '\0';
                    if(cCol > 2)
                        moveleft();
                }else
                    continue;
            }else if(ascii_ch == 10){ // '\n'
                movedown();
                //DO OTHER STUFF TO ALLOCATE NEWLIEN TO FULL_BUFFER
            }else{
                f_buf[start+cRow-2][cCol-2] = ch;
                moveright();
            }
            change_made = 1;
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
    printf("\033[2J");
    printf("\033[H");
    restore_input_mode(&oldtermios);
    return 0;
}


