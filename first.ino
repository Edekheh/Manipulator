
#include "pins.h"
#include <Arduino.h>
#include "menu.h"
String strInput="";
void(*menuFunc)(int);
void setup() {
 InitializeALL();
}

void loop() {
  while(Serial.available())  {
  strInput=Serial.readString();
  }
if(strInput.length()!=0) {
  int temp = strInput[0]-48;
  menuFunc(temp);
  strInput="";
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



