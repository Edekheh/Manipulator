#include "pins.h"
//#include <Servo.h> 

//Servo myservo;

void InitializeServos() {
    pinMode(SERVO1_PIN,OUTPUT);
    //myservo.attach(SERVO1_PIN);
}
int lenMicroSecondsOfPeriod = 25 * 1000;       // 25 milliseconds (ms). found much better smoothness at 25, not 20 ms.
               // used in the while loop below
int lenMicroSecondsOfPulseStart = 0.5 * 1000;  // 0 degrees
int lenMicroSecondsOfPulseEnd = 2.5 * 1000;    // 180 degrees
int lenMicroSecondsOfPulseStep = 0.01 * 1000;
int lenMicroSecondsOfPulse ; 
void openServoG10()  {

lenMicroSecondsOfPulse=lenMicroSecondsOfPulseStart + lenMicroSecondsOfPulseStep;
unsigned long startMilSec=millis();
    unsigned long currentMilSec;
 while (lenMicroSecondsOfPulse = lenMicroSecondsOfPulseStart)
 {
     currentMilSec=millis();
     if(startMilSec+600<currentMilSec) break;
   digitalWrite(SERVO1_PIN, HIGH);
   delayMicroseconds(lenMicroSecondsOfPulse);
   digitalWrite(SERVO1_PIN, LOW);
   delayMicroseconds(lenMicroSecondsOfPeriod - lenMicroSecondsOfPulse); 
   lenMicroSecondsOfPulse += lenMicroSecondsOfPulseStep;
 }
}

void closeServoG11()  {
lenMicroSecondsOfPulse=lenMicroSecondsOfPulseEnd + lenMicroSecondsOfPulseStep;
unsigned long startMilSec=millis();
    unsigned long currentMilSec;
 while (lenMicroSecondsOfPulse = lenMicroSecondsOfPulseStart)
 {
     currentMilSec=millis();
     if(startMilSec+600<currentMilSec) break;
   digitalWrite(SERVO1_PIN, HIGH);
   delayMicroseconds(lenMicroSecondsOfPulse);
   digitalWrite(SERVO1_PIN, LOW);
   delayMicroseconds(lenMicroSecondsOfPeriod - lenMicroSecondsOfPulse); 
   lenMicroSecondsOfPulse += lenMicroSecondsOfPulseStep;
 }
}