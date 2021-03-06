#include "interpreter.h"

#define DELIM " \t\r\n\a"
#define BUFFER 32

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
  Serial.println(tokens[0]);
    if(tokens==NULL)
      return -2;
    else if(!strcmp(tokens[0],"G0"))
      procedureG0(tokens);
    else if(!strcmp(tokens[0],"G1"))
      procedureG1(tokens);
    else if(!strcmp(tokens[0],"G2"))  {
    procedureG2(tokens);
    }
      
    else if(!strcmp(tokens[0],"G3"))  {
  procedureG3(tokens);
    }
    else if(!strcmp(tokens[0],"G4"))  {
       Serial.println(tokens[3]);
    procedureG3(tokens);
    }
      
    else if(!strcmp(tokens[0],"G5"))
      procedureG3(tokens);
    else if(!strcmp(tokens[0],"G10"))
      procedureG10();
    else if(!strcmp(tokens[0],"G11"))
      procedureG11();
    else if(!strcmp(tokens[0],"INIT"))  
      procedureInitialize();
    else if(!strcmp(tokens[0],"M0"))  
      procedureM0(tokens);
    free(tokens);
    return 0;
}