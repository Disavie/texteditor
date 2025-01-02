#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){

    FILE * f;

    if(argc != 2){
        printf("usage: canopen filename\n");
        exit(EXIT_FAILURE);
    }
    if((f = fopen(argv[1], "r")) == NULL){
        printf("%s can't be opened\n",argv[1]);
        exit(EXIT_FAILURE);
    }
    
    printf("%s can be opened\n",argv[1]);
    char line[100];
    char lines[100][100];
    int i = 0;
    int total_lines;
    while(!feof(f)){
        fgets(lines[i],sizeof(lines[0]),f); 
        i++;
    }
    total_lines = i;
    printf("%s has the contents:\n",argv[1]);
    i = 0;
    while(i < total_lines){
        printf("%s",lines[i]);
        i++;
    }
    fclose(f);
    


    return 0;
}
