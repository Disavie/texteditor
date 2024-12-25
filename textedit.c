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

    printf("%s",argv[1]);
    char temp;

    f = fopen(argv[1],"r");
    int linecount = countlines(f);
    fclose(f);

    int szHelper = linecount; 
    if(linecount < (int)HEIGHT){
        szHelper = HEIGHT;
    }
    char full_buffer[szHelper][WIDTH];

    for(int i = 0; i < szHelper;i++){
        for(int j = 0 ; j < WIDTH ; j++){
            full_buffer[i][j] = '\0';
        }
    }
    f = fopen(argv[1],"r");    
    int i = 0;
    while(!feof(f)){
        fgets(full_buffer[i],sizeof(full_buffer[0]),f);
        size_t len = strlen(full_buffer[i]);
        if(full_buffer[i][len-1] == '\n')
            full_buffer[i][len-1] = '\0';
        i++;
    }
    fclose(f);

    short rend_HEIGHT = HEIGHT - 3;

    short start = 0;
    short end = linecount;
    if(linecount < rend_HEIGHT) end = rend_HEIGHT; 


    int cRow = 2;
    int cCol = 2;
    int hasQuit = 0;
    int change_made = 1;
    while(!hasQuit){
        
        if(change_made){
            create_window_inoutRANGE(0,0,rend_HEIGHT,WIDTH,full_buffer,start,end);
            printf("\033[%d;%dH",cRow,cCol);
            change_made = 0;
        }

        ch = getchar();

        if(ch == '\0') continue; // no changes were made  
        if (ch == 'q'){
            hasQuit = 1;
        }else if (ch == '\033'){
            ch = getchar(); //skips the [ character
            if (ch == '['){

                ch = getchar(); //A/B/C/D
                switch (ch){
                    case 'A': moveup(); //SNAPLEFT ;break;
                    case 'B':
                        if(cRow <= linecount){ movedown(); }//SNAPLEFT }
                        break;
                    case 'C': moveright(); break;
                    case 'D': moveleft(); break;
                    default:
                        printf("unknown escape sequence: \\033[%c\n",ch);
                        char buf;
                        double dummy;
                        do{
                            buf = timed_input(0.01,&dummy);
                        }while(buf!= -1);
                        fflush(stdin);
                        continue;
                }
             }else
                  printf("unknown escape sequence: \\033%c",ch);
        }else{
            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(cCol > 2){
                    full_buffer[start+cRow-2][cCol-3] = '\0';
                    moveleft();
                }else
                    continue;
            }else if(ascii_ch == 10){ // '\n'
                movedown();
                //SNAP LEFT
                //DO OTHER STUFF TO ALLOCATE NEWLIEN TO FULL_BUFFER
            }else{
                full_buffer[start+cRow-2][cCol-2] = ch; //subtract 2 to account for the window borders
                moveright();
            }
            change_made = 1;
        }
        get_cursor_pos(&cRow,&cCol);
        fflush(stdin);
    }
    //saving the file
    f = fopen(argv[1],"w");
    if (f == NULL) printf("error opening file: %s\n",argv[1]);
    for(int i = 0 ; i < linecount ; i++){
        fprintf(f,"%s\n",full_buffer[i]);
    }
    fclose(f);


    restore_input_mode(&oldtermios);
    return 0;
}


