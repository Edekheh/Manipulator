#include "servo.h"
#include "pins.h"
Servo Servo1;

void InitializeServos() {
    Servo1.attach(SERVO1_PIN);
}

int translateServoDeg(int deg) {
    int finalValue;
    if(deg<90)  {
        finalValue=82;
    }
    else if(deg>90) {
        finalValue=96;
    }
    else    {
        finalValue=89;
    }
    return finalValue;
}

void movServo(int deg)    {
int finalDeg=translateServoDeg(deg);    
unsigned long startMilSec = millis();
//Serial.println(startMilSec);
//Serial.println(millis());
unsigned long currentMilSec;
while(1)    {
    currentMilSec=millis();
    if(startMilSec+1000<currentMilSec) break;
    Servo1.write(finalDeg);
}
Servo1.write(89);
//Serial.println(millis());
}