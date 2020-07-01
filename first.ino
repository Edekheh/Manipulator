#include "pins.h"
#include "formatText.h"
#include <SPI.h>
void setup() {
  definePinMode();
  InitializeServos();
  Serial.begin(15200);
  SPI.begin();
  printSetup();
}

void loop() {
while (Serial.available())  {
    SERIALPORTVALUE = Serial.readString();
  }
  if (SERIALPORTVALUE.length() > 2) {
    Serial.println("move:");
    translateString(SERIALPORTVALUE);
    SERIALPORTVALUE = "";
  }
  
}



