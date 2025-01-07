#include <unistd.h>
#include "mytui.h"
#include "Buffer.h"
#include "cursor.h"
#include "ansihelpers.h"
#include "colorscheme.h"
#include "normal.h"

#define STATUSBARHEIGHT 2

int main(int argc, char ** argv){

    openAltBuffer();
    clearLog();

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
    Buffer * mbuf = malloc(sizeof(Buffer));
    *mbuf =(Buffer) {
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
        loadFile(mbuf,argv[1]);
    }else{
        createFile(mbuf,NULL);
    }

    char statusBarMsg[128] = "";
    size_t sblen = 128;
    size_t cy = mbuf->yoffset+mbuf->ycorner;
    size_t cx = mbuf->xoffset+mbuf->xcorner;
    char ch = '\0';
    char mode = 'n'; 
    const short * colors  = zthemes[1].data;

    char temp[128];

    short isError = 0;
    short WINHEIGHT = HEIGHT - STATUSBARHEIGHT;
    short WINWIDTH = WIDTH;
    short STATUSBARPOS= HEIGHT;

    char lastsavetimestr[64];
    int filesize = calcsize(mbuf);
    get24Htime(lastsavetimestr,64);
    char * modestr = (char *)malloc(65*sizeof(char));
    strcpy(modestr,"--NORMAL--");
    strcpyf(statusBarMsg,sblen,"%s",modestr);


    drawbuffer(mbuf->ycorner,mbuf->xcorner,WINHEIGHT,WINWIDTH,mbuf,colors);
    movecurs(cy,cx);
    update_statusbar(statusBarMsg,STATUSBARPOS,WINWIDTH,mbuf,colors,cy,cx);
    if(argc == 1){movecurs((mbuf->linecount),(size_t)0); drawLogo(HEIGHT,WIDTH, colors); }
    movecurs(cy,cx);
    showcursor();

    
    char input_buffer[8];
    memset(input_buffer,'\0',sizeof(input_buffer));
    int tabwidth = 4;
    //motion anatomy prefix (d y)
    //count
    //motion type (w f<char> $ ^)

    ssize_t bytes_read;
    Buffer ** history = initbarr();
    copyBuffer(history[0],mbuf);
    
    int made_edit = 0;
    size_t his_sz = 1; 
    size_t his_i = 0;
    int quit = 0;

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
            strcpyf(statusBarMsg,sblen,"%s",modestr);
            update_statusbar(statusBarMsg,STATUSBARPOS,WINWIDTH,mbuf,colors,cy,cx);
            continue;
        }

        if (ch == '\033'){
            if(input_buffer[1] == '\0'){
                mode = 'n';
                if(made_edit) {
                    if(his_i == his_sz -1){
                        addbuffer(&history,&his_sz,mbuf);
                        his_i++;
                    }else{
                        clearbarr_fromi(&history,&his_sz,his_i);
                        addbuffer(&history,&his_sz,mbuf);
                        his_i++;
                    }
                    made_edit = 0;
                }
                
                strcpy(modestr,"--NORMAL--");
                strcpyf(statusBarMsg,sblen,"%s",modestr);
                update_statusbar(statusBarMsg,STATUSBARPOS,WINWIDTH,mbuf,colors,cy,cx);
                continue;

                if(mbuf->xpos+cx-mbuf->xoffset == strlen(mbuf->contents[mbuf->ypos+cy-mbuf->yoffset])){
                    smart_moveleft(mbuf,cx);
                }else; 

            }else if(input_buffer[1] == '['){
                cursorMovement(mbuf,input_buffer[2],cy,cx,mode,WINHEIGHT, WINWIDTH);
            }
        }

        //varibles in normalmain 
        //&mbuf &cy &cx WINHEIGHT, WINWIDTH &undohistory
        //extras : &statusBarMsg, sblen, &modestr, &mode, &filesize, &hasSaved, &lastsavetimestr, &statusbarpos
        //colors
        //                 char *       s_t *  char **  char * size_t * int *     char *          short *
        void * extras[] = {statusBarMsg,&sblen,&modestr,&mode,&filesize,&hasSaved,lastsavetimestr,&STATUSBARPOS,
        //                 size_t * size_t *
                           &his_i,&his_sz,&made_edit, 
                            };
        
        if(mode == 'n'){
            //checks for motions and commands
            if(normalmain(ch, &mbuf,&cy,&cx,WINHEIGHT,WINWIDTH,&history,extras,&colors)){
                quit = 1;
            }
            if(quit)break;
    
        }else if(mode == 'i'){

            short ascii_ch = (short)ch;
            if(ascii_ch == 127){ //ascii 127 is backspace
                if(mbuf->xpos+cx-mbuf->xoffset== 0 && mbuf->ypos+cy-mbuf->yoffset!= 0){ //CASE OF DELETING LINE

                    char * removed_line = remove_line(mbuf,mbuf->ypos+cy-mbuf->yoffset);
                    smart_moveup(mbuf,cy);
                    get_cursor_pos(&cy,&cx);

                    snapCursorRight(mbuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
                    get_cursor_pos(&cy,&cx);


                    if(removed_line[0] != '\0'){
                        char * aboveline = mbuf->contents[mbuf->ypos+cy-mbuf->yoffset]; 
                        size_t alen = strlen(aboveline);
                        size_t rlen = strlen(removed_line);
                        char * newabove= (char *)realloc(aboveline,(alen+rlen+1)*sizeof(char));
                        aboveline = newabove;
                        mbuf->contents[mbuf->ypos+cy-mbuf->yoffset] = aboveline;
                        strcat(aboveline,removed_line);
                        free(removed_line);
                    }else ;
                }else{
                    if(mbuf->xpos+cx-mbuf->xoffset== 0 && mbuf->ypos+cy-mbuf->yoffset == 0) continue;
                    size_t row = mbuf->ypos+cy-mbuf->yoffset;
                    int col = mbuf->xpos+cx-mbuf->xoffset;
                    if(mbuf->contents[row][col-1] == '\t'){

                        remove_from_line(mbuf,mbuf->ypos+cy-mbuf->yoffset,col-1);
                        smart_moveleft(mbuf,cx);
                        get_cursor_pos(&cy, &cx);
                        while(mbuf->xpos+cx-mbuf->xoffset > 0 && mbuf->contents[mbuf->ypos+cy-mbuf->yoffset][mbuf->xpos+cx-mbuf->xoffset-1] != '\t' && !isalnum(mbuf->contents[mbuf->ypos+cy-mbuf->yoffset][mbuf->xpos+cx-mbuf->xoffset-1])){
                            remove_from_line(mbuf,mbuf->ypos+cy-mbuf->yoffset,mbuf->xpos+cx-mbuf->xoffset-1);
                            smart_moveleft(mbuf,cx);
                            get_cursor_pos(&cy, &cx);
                        }
                    }else{
                        remove_from_line(mbuf,row,col-1);
                        smart_moveleft(mbuf,cx);
                    }
                }
            }else if(ascii_ch == 10){ // '\n'


                size_t copy_start = mbuf->xpos+cx-mbuf->xoffset;
                size_t line_index = mbuf->ypos+cy-mbuf->yoffset;
                char * line = mbuf->contents[line_index];
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

                    mbuf->contents[line_index] = shortened_line;
                    //Insert line
                    insert_line(mbuf,newl,line_index+1);
                }else{
                    char * line = "\0";
                    insert_line(mbuf,line,line_index+1);
                }
                smart_movedown(mbuf,cy,WINHEIGHT);
                get_cursor_pos(&cy,&cx);
                movecurs(cy,(size_t)mbuf->xoffset);
            }else if(ch == '\t'){ //\t

                size_t row = mbuf->ypos+cy-mbuf->yoffset;
                size_t col = mbuf->xpos+cx-mbuf->xoffset;

                for(int i = 0 ; i < tabwidth-1 ; i++){

                    insert_to_line(mbuf,row,col,' ');
                    smart_moveright_i(mbuf,cx, strlen(mbuf->contents[row]),WINWIDTH); 
                }
                insert_to_line(mbuf,row,col+tabwidth-1,'\t');
                smart_moveright_i(mbuf,cx, strlen(mbuf->contents[row]),WINWIDTH); 


            }else{
                if(ascii_ch >= 32){ //Real ASCII chracters start at 32 with [space] with the exception of \t \n 
                    insert_to_line(mbuf,mbuf->ypos+cy-mbuf->yoffset,mbuf->xpos+cx-mbuf->xoffset,ch);
                    smart_moveright_i(mbuf,cx, strlen(mbuf->contents[cy+mbuf->ypos-mbuf->yoffset]),WINWIDTH); 
                }

            }
            hasSaved = 0;
            made_edit = 1;
        }

        get_cursor_pos(&cy,&cx);
        drawbuffer(mbuf->ycorner,mbuf->xcorner,WINHEIGHT,WINWIDTH,mbuf,colors);
        movecurs(cy,cx);
        update_statusbar(statusBarMsg,STATUSBARPOS,WINWIDTH,mbuf,colors,cy,cx);
        memset(input_buffer,'\0',sizeof(input_buffer));
        snapCursorLeft(mbuf,&cy,&cx,WINHEIGHT,WINWIDTH,colors);
        get_cursor_pos(&cy,&cx);
        showcursor();

    }
    clearbarr(&history,&his_sz);
    freeBuffer(mbuf);
    free(modestr);

    resetColor();
    restore_input_mode(&oldtermios);
    closeAltBuffer();
    showcursor();
    return 0;
}
