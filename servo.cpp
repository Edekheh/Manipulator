#include "pins.h"
#include <Servo.h>

Servo servo1;

void InitializeServos()
{
    //pinMode(SERVO1_PIN, OUTPUT);
    servo1.attach(SERVO1_PIN);
}
void closeServoG11()
{

    //unsigned int l = 1600;
    //unsigned int fl = 20000;
    unsigned long startMilSec = millis();
    unsigned long currentMilSec=millis();
    while (startMilSec + 400 > currentMilSec)
    {
        servo1.write(95);
        currentMilSec = millis();
        /*digitalWrite(SERVO1_PIN, HIGH);
         delayMicroseconds(l);
        digitalWrite(SERVO1_PIN, LOW);
        delay(18);
        delayMicroseconds(400);*/
    }
}

void openServoG10()
{
    unsigned int l = 1200;
    unsigned int fl = 20000;
    unsigned long startMilSec = millis();
    unsigned long currentMilSec=millis();
    while (startMilSec + 400 > currentMilSec)
    {
        servo1.write(85);
        currentMilSec = millis();
        /*digitalWrite(SERVO1_PIN, HIGH);
        delayMicroseconds(l);
        digitalWrite(SERVO1_PIN, LOW);
        delay(18);
        delayMicroseconds(900);*/
    }
}