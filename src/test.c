#include "mytui.h"
#include "colorscheme.h"


int main(){

    size_t len = strlen("\033[38;5;1mblue\033[38;5;0m");
    printf("%lu\n",len);
    const char * s = "int main burger cheeese return 5;";
    const char * s2 = "\033[38;5;39mint\033[38;5;255m main burger cheeese return 5;";
    //                  1  234567890123
    printf("\\%c\\",s2[15]);
    char * sc = (char *)malloc((strlen(s)+1)*sizeof(char));
    strcpy(sc,s);
    sc[strlen(s)] = '\0';

    char * new = highlightsubstr(sc,2,5,default2);
    char * other = highlightLine(sc,default2);
    printf("%s\n",new);
    printf("%s\n",other);




}
