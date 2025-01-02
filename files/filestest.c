#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

    FILE * f;
    

    //count lines
    char ch;

    f = fopen("test2.txt","r");
    int linecount = 0;
    int nonemptyline = 0;
    while((ch = fgetc(f))!=EOF){
        if ( ch == '\n' ){
            linecount++;
            if(nonemptyline){
                nonemptyline = 0;
            }
        }else if(ch != ' ' &&  ch != '\t' && nonemptyline == 0){
            nonemptyline = 1;
        }
    }
    if(nonemptyline){
        linecount++;
    }

    fclose(f);
    printf("file is %d lines line \n",linecount);


    f = fopen("test2.txt","w");    
    int hasQuit = 0;

    while(!hasQuit){
    
        char line[100];
        fgets(line,sizeof(line),stdin);
        
        if(line[0] == 'q' && line[1] == '!'){
            hasQuit = 1;
        }
        fprintf(f,"%s",line);





    }

    fclose(f);

    return 0;
}
