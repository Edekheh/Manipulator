#include "interpreter.h"
#include <string.h>
#include <stdlib.h>

#define DELIM " \t\r\n\a"
#define BUFFER 16

char** parse(char line[]){

  int bufsize = BUFFER, position = 0;
  char **tokens = (char**)malloc(bufsize * sizeof(char*));
  char *token;

  if (tokens==NULL) {
    return NULL;
  }
  token = strtok(line, DELIM);
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= bufsize) {
      bufsize +=  BUFFER;
      tokens = (char**)realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        return NULL;
      }
    }
    token = strtok(NULL, DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int interpreter(char input[]){
    
    char **tokens;

    tokens=parse(input);

    if(tokens==NULL)
      return -1;

    if(tokens[0]=="G0"){
      procedureG0(tokens);
    }else{
      Serial.println("Invalid or not supported command");
    }
    return 0;
}