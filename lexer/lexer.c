#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    NONE = 0, 
    DATA_TYPE = 1, //int float double char void size_t short long
    CONTROL_FLOW = 2, //if else for while do goto break continue switch case default
    FUNCTION = 3, //return
    MISC = 4, //const typedef struct
    NUMBER = 5,
    STRING = 6,
    COMMENT = 7,
    PREPROCESSOR = 8
}keyword;

const char * DATA_TYPE_KEYWORDS[] = {
    "int","float","double","char","void","size_t","short","long","auto"
};
const char * CONTROL_FLOW_KEYWORDS[] = {
    "if","else","for","while","do","goto","break","continue","switch","case"
};
const char * FUNCTION_KEYWORDS[] = {
    "return"
};
const char * MISC_KEYWORDS[] = {
    "const","typedef","struct"
};


char * build_word(char * line, size_t *index){
    size_t start = *index;
    size_t len = 0;

    while(isspace(line[start])){
        start++;
        (*index)++;
    }

    size_t current = start;

    while(line[current] != '\0' && (isalnum(line[current]) || line[current] == '_')){ 
        current++;
        len++;
        (*index)++;
    }
    char * word = (char *)malloc((len+1)*sizeof(char));
    strncpy(word,line+start,len);
    word[len] = '\0';
    return word;
}

int isnumber(char * word){

    if(strlen(word) == 0)
        return 0;

    for(size_t i = 0 ; i < strlen(word) ; i++) {
        if(!isdigit(word[i]))
            return 0;
    }

    return 1;
}

keyword iskeyword(char * word){

    keyword key;
    
    if(isnumber(word))
        return NUMBER;

    for(int i = 0 ; i < sizeof(DATA_TYPE_KEYWORDS)/sizeof(DATA_TYPE_KEYWORDS[0]);i++){
        if(strcmp(word,DATA_TYPE_KEYWORDS[i]) == 0 ){
            return DATA_TYPE;
        }
    }

    for(int i = 0 ; i < sizeof(CONTROL_FLOW_KEYWORDS)/sizeof(CONTROL_FLOW_KEYWORDS[0]);i++){
        if(strcmp(word,CONTROL_FLOW_KEYWORDS[i]) == 0 ){
            return CONTROL_FLOW;
        }
    }

    for(int i = 0 ; i < sizeof(FUNCTION_KEYWORDS)/sizeof(FUNCTION_KEYWORDS[0]);i++){
        if(strcmp(word,FUNCTION_KEYWORDS[i]) == 0 ){
            return FUNCTION;
        }
    }

    for(int i = 0 ; i < sizeof(MISC_KEYWORDS)/sizeof(MISC_KEYWORDS[0]);i++){
        if(strcmp(word,MISC_KEYWORDS[i]) == 0 ){
            return MISC;
        }
    }

    return NONE;
}



typedef struct{
    short keywordtype;
    size_t index;
    char * text;
    size_t true_length; //length INCLUDING \033...
    size_t length; //length EXCLUDING \033...
}Token;


void changetext(Token *token, short color) {
    char col_str[16];
    snprintf(col_str, sizeof(col_str), "%d", color);

    size_t new_len = strlen(token->text) + 16 + strlen(col_str);
    char *newtext = (char *)malloc(new_len + 1); // +1 for null terminator

    snprintf(newtext, new_len + 1, "\033[38;5;%sm%s\033[39m", col_str, token->text);
    token->true_length = strlen(newtext);

    free(token->text);
    token->text = newtext;
}
void colortokens(Token * tokens, size_t tokencount){

    for(size_t i = 0 ; i < tokencount ; i++) {
        if(tokens[i].keywordtype == DATA_TYPE){
            changetext(tokens+i,62);
        }else if(tokens[i].keywordtype == CONTROL_FLOW){
            changetext(tokens+i,39);
        }else if(tokens[i].keywordtype == FUNCTION){
            changetext(tokens+i,9);
        }else if(tokens[i].keywordtype == MISC){
            changetext(tokens+i,208);
        }else if(tokens[i].keywordtype == NUMBER){
            changetext(tokens+i,5);
        }
    }
}

char remove_from_line(char ** line, size_t index){

    char ch = (*line)[index];
    size_t len = strlen(*line);

    for(size_t i = index; i < len ; i++){
        (*line)[i] = (*line)[i+1];
    }
    char * newline= (char *)realloc(*line,len*sizeof(char));
    *line = newline;


    return ch;
}

void insert_to_line(char ** line,size_t index,char ch){
    size_t len = strlen(*line);

    char * newline = (char *)realloc(*line,(len+2)*sizeof(char));


    for(size_t i = len; i > index ; i--){
        (*line)[i] = (*line)[i-1];
    }

    *line = newline;
    (*line)[len+1] = '\0';
    (*line)[index] = ch;        
}
void remove_range(char **line, size_t index, size_t length) {
    memmove(*line + index, *line + index + length, strlen(*line) - index - length + 1);
}

void insert_word(char **line, char *insert, size_t index) {
    size_t len_line = strlen(*line);
    size_t len_insert = strlen(insert);

    char *newline = (char *)malloc(len_line + len_insert + 1);
    strncpy(newline, *line, index);
    strcpy(newline + index, insert);
    strcpy(newline + index + len_insert, *line + index);

    free(*line);
    *line = newline;
}
void reinsert_tokens(Token * tokens, size_t tokencount,char ** line){

    int index_helper = 0; 
    for(int i = 0 ; i < tokencount ; i++){
        Token token = tokens[i];
        //printf("1 \t %s\n",line);
        remove_range(line,token.index+index_helper,token.length);
        //printf("2 \t %s\n",line);
        insert_word(line,token.text,token.index+index_helper);
        index_helper+=(token.true_length-token.length);
    }
}

int main(int argc, char ** argv){


    static char src2[32][64] = {
        "int main(int argc, char **argv) {",
        "    char src[32][32] = {",
        "        \"int main(int argc, char **argv) {\",",
        "        \"    char src[32][32] = {\",",
        "        \"    };\"",
        "    };",
        "    for (int i = 0; i < 32; i++) {",
        "        for (int j = 0; j < 32; j++) {",
        "            printf(\\\"%s\\\", src[i][j]);",
        "        }",
        "        printf(\\\"\\n\\\");",
        "    }",
        "    return 0;",
        "}",
        "}"
    };
    char ** src = (char **)malloc(32*sizeof(char*));
    for(int i = 0 ; i < 32 ; i++) {
        src[i] = (char *)malloc(64*sizeof(char));
        strcpy(src[i],src2[i]);
    }

    Token * tokens = NULL; 
    size_t tokencount = 0;

    size_t i  = 0;
    int j = 0;
    while(j < 16){
        i = 0;
        while(i < strlen(src[j])){

            char * word = build_word(src[j],&i);
            keyword type;
            if((type = iskeyword(word))){
                Token token = {
                    .keywordtype = type,
                    .index = i-strlen(word),
                    .length = strlen(word),
                };
                token.text = (char *)malloc((strlen(word))*sizeof(char));
                strcpy(token.text,word);

                Token * new_tokens = (Token *)realloc(tokens,(tokencount+1) * sizeof(Token));
                tokens = new_tokens;
                tokens[tokencount] = token;
                tokencount++;

            }
            i++;
            free(word);
        }

        colortokens(tokens,tokencount);
        reinsert_tokens(tokens,tokencount,src+j);
        printf("%s\n",src[j]);
        tokens = NULL;
        tokencount = 0;
        j++;
    }

    return 0;
}
