#include "mytui.h"
#include <stdio.h>

int main(int argc, char ** argv){

    struct termios old_termios;
    set_input_mode(&old_termios);
    setbuf(stdout,NULL);

    char buffer[8];
    ssize_t bytes_read = 0;
    while((bytes_read = read(STDIN_FILENO,buffer,sizeof(buffer))) != -1){
        if(buffer[0] == 'q') break;
        printf("%c",buffer[0]);

    }

    restore_input_mode(&old_termios);
    return 0;
}
