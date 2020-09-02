#include "interpreter.h"
#include "pins.h"
#include <Arduino.h>
#include <stdlib.h>
#include "menu.h"
String strInput=0;
void(*menuFunc)(int);
void setup() {
 InitializeALL();
}

void loop() {
  while(Serial.available())  {
  strInput=Serial.readString();
  }
if(strInput.length()!=0) {
  int temp = strInput[0]-65;
  menuFunc(temp);
  strInput=0;
}
/*if(strInput.length()>2) {
char *input = new char[40];
  strInput.toCharArray(input,strInput.length());
  Serial.println(strInput);
  if(interpreter(input)==-1)
    Serial.println("Null error or invalid command");
  strInput="";
}*/
}

void InitializeALL()  {
 definePinMode();
  procedureInitialize();
  Serial.begin(115200);
InitializeServos();
initializeMenuOptions();
}



