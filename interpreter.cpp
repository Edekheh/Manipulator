#include "interpreter.h"
#include <string.h>

#define DELIM " \t\r\n\a"
#define BUFFER 32

void printSetup()
{
  Serial.println("Moving robotic arm by pseudoGCode");
  Serial.println("G1 - moving command, A B C D E F - axis names, degree from 000 to 180");
  Serial.println("Example : G1 A32 ;");
  Serial.println("^^^Moving A axis by 32 degrees^^^");
}
char **parse(String str){

  char **tokens = malloc(BUFFER * sizeof(char*));
  char *token = strtok(str, DELIM);
  int position = 0;
  int bufsize = BUFFER;
  //error handling
  if (tokens==NULL) {
    Serial.println("! ! ! ! ! !\nallocation error\n! ! ! ! ! !\n");
    exit(EXIT_FAILURE);
  }
  while (token != NULL) {
    tokens[position] = token;
    position++;
    //does string exceed buffer size ? if yes reallocate
    if (position >= bufsize) {
      bufsize += BUFFER;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        Serial.println("! ! ! ! !\nreallocation error\n! ! ! ! !\n");
        exit(EXIT_FAILURE);
      }
    }
    //end of reallocation, error handling done
    token = strtok(NULL, DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}
void interpreter(){
    
    char **tokensCommand;

    printSetup();
    while(1){

        tokensCommand=Parse(Serial.readString())

        switch(*tokensCommand[0]){
          case "G0":
            //TODO
          case "G1":
            //TODO
          case "G2":
            //TODO
          case "G3":
            //TODO
          case "G4":
            //TODO
          case "G5":
            //TODO
          default:
            Serial.println("Command not supported or not valid");
        }

    }
}