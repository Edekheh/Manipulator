#include "stepper.h"
#include "states.h"
#include "servo.h"
#include "interpreter.h"
#include <string.h>

#define DELIM " \t\r\n\a"
#define BUFFER 32
//axis one (X) -> 0
//axis two (Y) -> 1
//axis three (Z) -> 2
//axis four (E0) -> 3
//axis five (E1) -> 4
//G0 G1 G2 G3 G4 G5 M-ileś zmiana prędkości przegubów
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

        if(*tokensCommand[0]="G0")
          
        else if(*tokensCommand[0]="G2")

        else if(*tokensCommand[0]="G3")

        else if(*tokensCommand[0]="G4")

        else if(*tokensCommand[0]="G5")

    }
}