#include "pins.h"
//Servo Servo1;

void InitializeServos() {
    pinMode(SERVO1_PIN,OUTPUT);
}

void openServoG10()  {
    unsigned long startMilSec=millis();
    unsigned long currentMilSec;
    Serial.println("Opening gripper...");
    while(startMilSec+800<currentMilSec)    {
        currentMilSec=millis();
        digitalWrite(SERVO1_PIN,HIGH);
        delayMicroseconds(1750);
    }

}

void closeServoG11()  {
    unsigned long startMilSec=millis();
    unsigned long currentMilSec;
    Serial.println("Closing gripper...");
    while(startMilSec+800<currentMilSec)    {
        currentMilSec=millis();
        digitalWrite(SERVO1_PIN,HIGH);
        delayMicroseconds(1250);
    }
}