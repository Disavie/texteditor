#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "mytui.h"
#include <stdbool.h>


typedef enum {
    NONE = 0, 
    DATA_TYPE = 1, //int float double char void size_t short long
    CONTROL_FLOW = 2, //if else for while do goto break continue switch case default
    FUNCTION = 3, //return
    MISC = 4, //const typedef struct
    NUMBER = 5,
    STRING = 6,
    COMMENT = 7,
    PREPROCESSOR = 8,
    ANGLE_BRACKETS = 9,

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
    "const","typedef","struct","NULL"
};

char * build_word(char * line, size_t *index) {
    size_t start = *index;
    size_t len = 0;

    // Skip any leading white spaces
    while (isspace(line[start])) {
        start++;
        (*index)++;
    }

    size_t current = start;

    // Handle content inside quotes (single or double)
    if (line[current] == '"' || line[current] == '\'') {
        char quote_type = line[current];
        current++;
        (*index)++;
        len++;
        while (line[current] != quote_type && line[current] != '\0') {
            current++;
            len++;
            (*index)++;
        }
        if (line[current] != '\0') {
            current++;
            len++;
            (*index)++;
        }
    } 
    // Handle content inside angle brackets <>
    else if (line[current] == '<') {
        current++;
        (*index)++;
        len++;
        while (line[current] != '>' && line[current] != '\0') {
            current++;
            len++;
            (*index)++;
        }
        if (line[current] == '>') {
            current++;
            len++;
            (*index)++;
        }
    }
    // Handle single-line comments (starting with //)
    else if (line[current] == '/' && line[current + 1] == '/') {
        current++;
        (*index)++;
        len++;
        while (line[current] != '\0') {
            current++;
            (*index)++;
            len++;
        }
    } 
    // Handle preprocessor directives (starting with #)
    else if (line[current] == '#') {
        current++;
        (*index)++;
        len++;
        // Capture until < or " are encountered, but do not include them
        while (line[current] != '\0' && line[current] != '\n' && line[current] != '<' && line[current] != '"') {
            current++;
            len++;
            (*index)++;
        }
    } 
    // Handle regular words consisting of alphanumeric characters or underscores
    else {
        current++;
        len++;
        (*index)++;
        while (line[current] != '\0' && (isalnum(line[current]) || line[current] == '_')) {
            current++;
            len++;
            (*index)++;
        }
    }

    // Allocate memory for the word and copy it
    char * word = (char *)malloc((len + 1) * sizeof(char));
    strncpy(word, line + start, len);
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
    
    if(isnumber(word)){
        return NUMBER;
    }else if(word[0] == '\'' || word[0] == '"'){
        return STRING;
    }else if(word[0] == '<'){
        return ANGLE_BRACKETS;
    }else if(word[0] == '/' && word[1] == '/'){
        return COMMENT;
    }else if(word[0] == '#'){
        return PREPROCESSOR;
}

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
    short fcode_len;
    short bcode_len;
}Token;


void changetext(Token *token, short color,const short colors[]) {

    size_t new_len = strlen(token->text) + 32;
    char *newtext = (char *)malloc(new_len + 1); // +1 for null terminator
    memset(newtext,'\0',new_len);

    snprintf(newtext, new_len + 1, "\033[38;5;%dm%s\033[38;5;%dm", color, token->text,colors[0]);
    token->true_length = strlen(newtext);
    free(token->text);
    token->text = newtext;
}

size_t numlen(int num){

    size_t len = 1;
    if(num == 0) return len;
    while(num > 0){
        len++;
        num/=10;
    }
    return len;
}


void colortokens(Token * tokens, size_t tokencount,const short colors[]){


    for(size_t i = 0 ; i < tokencount ; i++) {
        
        size_t code_length = 0;
        switch (tokens[i].keywordtype){

        case(DATA_TYPE):
            changetext(tokens+i,colors[9],colors);
            code_length = 8 + numlen(colors[9]); 
            break;
        case(CONTROL_FLOW):
            changetext(tokens+i,colors[10],colors);
            code_length = 8 + numlen(colors[10]); 
            break;
        case(FUNCTION):
            changetext(tokens+i,colors[11],colors);
            code_length = 8 + numlen(colors[11]); 
            break;
        case(MISC):
            changetext(tokens+i,colors[12],colors);
            code_length = 8 + numlen(colors[12]); 
            break;
        case(NUMBER):
            changetext(tokens+i,colors[15],colors);
            code_length = 8 + numlen(colors[15]); 
            break;
        case(COMMENT):
            changetext(tokens+i,colors[3],colors);
            code_length = 8 + numlen(colors[3]); 
            break;
        case(PREPROCESSOR):
            changetext(tokens+i,colors[14],colors);
            code_length = 8 + numlen(colors[14]); 
            break;
        case(STRING):
            changetext(tokens+i,colors[13],colors);
            code_length = 8 + numlen(colors[13]); 
            break;
        case(ANGLE_BRACKETS):
            changetext(tokens+i,colors[16],colors);
            code_length = 8 + numlen(colors[16]); 
            break;
        }
        tokens[i].fcode_len = code_length;
        tokens[i].bcode_len = 8 + numlen(colors[0]);
    }
}

void remove_range(char **line, size_t index, size_t length) {
    memmove(*line + index, *line + index + length, strlen(*line) - index - length + 1);
}

void insert_word(char **line, char *insert, size_t index) {

    size_t len_line = strlen(*line);
    size_t len_insert = strlen(insert);

    char *newline = (char *)malloc(len_line + len_insert + 1);
    if(newline != NULL){
        strncpy(newline, *line, index);
        strcpy(newline + index, insert);
        strcpy(newline + index + len_insert, *line + index);
        free(*line);
        *line = newline;
    }
}


size_t reinsert_tokens(Token * tokens, size_t tokencount,char ** line){

    size_t index_helper = 0; 
    for(int i = 0 ; i < tokencount ; i++){
        Token token = tokens[i];
        remove_range(line,token.index+index_helper,token.length);
        insert_word(line,token.text,token.index+index_helper);
        index_helper+=(token.true_length-token.length);
        free(token.text);
    }
    return index_helper;
}

size_t countTokenOffset(Token * tokens, size_t amount){

    int i = 0;
    size_t total = 0;
    while(i < amount){
        total = total + (tokens[i].fcode_len) + (tokens[i].bcode_len);
    }
    return total;
}



bool is_ansi_code(const char *str, size_t *length) {
    if (str[0] == '\033' && str[1] == '[') {
        *length = 2; 
        while (str[*length] && (str[*length] < '@' || str[*length] > '~')) {
            (*length)++;
        }
        if (str[*length]) {
            (*length)++;
            return true;
        }
    }
    return false;
}

char *extract_ansi_preserving_substring(const char *s2, size_t start, size_t end) {
    size_t logical_index = 0;  // Logical position (ignoring ANSI codes)
    size_t i = 0;             // Iterator for s2
    size_t ansi_length = 0;   // Length of ANSI code if found
    size_t buffer_size = strlen(s2) + 1;
    char *result = malloc(buffer_size);
    size_t result_index = 0;

    if (!result) {
        fprintf(stderr, "Memory allocation failed.\n");
        return NULL;
    }

    while (s2[i] && logical_index <= end) {
        if (is_ansi_code(&s2[i], &ansi_length)) {
            // Copy the ANSI code to result
            strncpy(&result[result_index], &s2[i], ansi_length);
            result_index += ansi_length;
            i += ansi_length; // Skip the ANSI code
        } else {
            if (logical_index >= start && logical_index <= end) {
                result[result_index++] = s2[i];
            }
            i++;
            logical_index++;
        }
    }

    result[result_index] = '\0'; // Null-terminate the resulting string
    return result;
}


char * highlightsubstr(char *line,size_t start, size_t end, const short colors[]){
    if(line == NULL || strlen(line) == 0) return NULL;
    if(end > strlen(line)) end = strlen(line);

    Token * tokens = NULL; 
    size_t tokencount = 0;
    char * highlighted = (char *)malloc((strlen(line)+1)*sizeof(char));
    strcpy(highlighted,line);
    highlighted[strlen(line)] = '\0';

    size_t i  = 0;
    int loops = -1;
    while(i < strlen(line)){

        char * word = build_word(highlighted,&i);

        keyword type;
        if((type = iskeyword(word))){
            Token token = {
                .keywordtype = type,
                .index = i-strlen(word),
                .length = strlen(word),
            };
            token.text = (char *)malloc((strlen(word)+1)*sizeof(char));
            strcpy(token.text,word);
            token.text[strlen(word)] = '\0';
            Token * new_tokens = (Token *)realloc(tokens,(tokencount+1) * sizeof(Token));
            tokens = new_tokens;
            tokens[tokencount] = token;
            tokencount++;
        }
        free(word);
    }

    colortokens(tokens,tokencount,colors);
    reinsert_tokens(tokens,tokencount,&highlighted);

    char * start_ptr = highlighted+start;
    char * end_ptr = highlighted+end;

    char * sub =  extract_ansi_preserving_substring(highlighted,start,end);

    free(tokens);
    free(highlighted);

    return sub;
}


char * highlightLine(char *line,const short colors[]){
    if(line == NULL || strlen(line) == 0) return NULL;

    Token * tokens = NULL; 
    size_t tokencount = 0;
    char * highlighted = (char *)malloc((strlen(line)+1)*sizeof(char));
    strcpy(highlighted,line);
    highlighted[strlen(line)] = '\0';


    size_t i  = 0;
    while(i < strlen(line)){

        char * word = build_word(highlighted,&i);

        keyword type;
        if((type = iskeyword(word))){
            Token token = {
                .keywordtype = type,
                .index = i-strlen(word),
                .length = strlen(word),
            };
            token.text = (char *)malloc((strlen(word)+1)*sizeof(char));
            strcpy(token.text,word);
            token.text[strlen(word)] = '\0';
            Token * new_tokens = (Token *)realloc(tokens,(tokencount+1) * sizeof(Token));
            tokens = new_tokens;
            tokens[tokencount] = token;
            tokencount++;

        }
        free(word);
        i++;
    }
    colortokens(tokens,tokencount,colors);
    reinsert_tokens(tokens,tokencount,&highlighted);

    for(int i = 0 ; i < tokencount ; i++)
        free(tokens[i].text);
    free(tokens);

    return highlighted;
}


