#include "stepper.h"
#include "pins.h"
//stepper motor controlls
int STEP_PINS[5] = {X_STEP_PIN, Y_STEP_PIN, Z_STEP_PIN, E0_STEP_PIN, E1_STEP_PIN};
int DIR_PINS[5] = {X_DIR_PIN, Y_DIR_PIN, Z_DIR_PIN, E0_DIR_PIN, E1_DIR_PIN};
int ENABLE_PINS[5];
int AXIS_STEPS_PER_REV[5] = {3200, 3200, 3200, 3200, 3200}; //microstepps
float AXIS_MAX_REV[5] = {5, 3.2, 9, 1, 1.8};
int AXIS_POS[5] = {90, 90, 90, 180, 90};
int AXIS_DEGREES[5] = {180, 180, 180, 360, 180};
int AXIS_SPEED[5] = {400, 300, 200, 300, 500};
bool AXIS_DIRECTION[5] = {1, 0, 0, 0, 1}; //1 for inverse logic
float AXIS_STEPS_PER_DEGREE[5] = {(AXIS_STEPS_PER_REV[0] * AXIS_MAX_REV[0]) / AXIS_DEGREES[0],
                                  (AXIS_STEPS_PER_REV[1] * AXIS_MAX_REV[1]) / AXIS_DEGREES[1],
                                  (AXIS_STEPS_PER_REV[2] * AXIS_MAX_REV[2]) / AXIS_DEGREES[2],
                                  (AXIS_STEPS_PER_REV[3] * AXIS_MAX_REV[3]) / AXIS_DEGREES[3],
                                  (AXIS_STEPS_PER_REV[4] * AXIS_MAX_REV[4]) / AXIS_DEGREES[4]};
bool checkDeg(int axis, int deg)
{
    if ((AXIS_POS[axis] + deg < 0) || (AXIS_POS[axis] + deg > AXIS_DEGREES[axis]))
        return 0;
    return 1;
}
bool checkPos(int axis, int pos)
{
    //TODO
}
void mov0(int axis, int deg)
{
    if (checkDeg(axis, deg))
    {
        if (deg < 0)
        {
            digitalWrite(DIR_PINS[axis], !AXIS_DIRECTION[axis]);
        }
        else
        {
            digitalWrite(DIR_PINS[axis], AXIS_DIRECTION[axis]);
        }
        int tempDeg = abs(deg);
        tempDeg = tempDeg * AXIS_STEPS_PER_DEGREE[axis];
        for (int i = 0; i < tempDeg; i++)
        {
            digitalWrite(STEP_PINS[axis], HIGH);
            delayMicroseconds(AXIS_SPEED[axis]);
            digitalWrite(STEP_PINS[axis], LOW);
            delayMicroseconds(AXIS_SPEED[axis]);
        }
        AXIS_POS[axis] += deg;
    }
}

void writeDirPin(int axis, int pos)
{
    if (pos > AXIS_POS[axis])
    {
        digitalWrite(DIR_PINS[axis], AXIS_DIRECTION[axis]);
    }
    else if (pos < AXIS_POS[axis])
    {
        digitalWrite(DIR_PINS[axis], !AXIS_DIRECTION[axis]);
    }
}
void mov1(int axis, int pos)
{
    int tempPos = abs(pos - AXIS_POS[axis]);
    writeDirPin(axis, pos);
    for (int i = 0; i < AXIS_STEPS_PER_DEGREE[axis] * tempPos; i++)
    {
        digitalWrite(STEP_PINS[axis], HIGH);
        delayMicroseconds(AXIS_SPEED[axis]);
        digitalWrite(STEP_PINS[axis], LOW);
        delayMicroseconds(AXIS_SPEED[axis]);
    }
    AXIS_POS[axis] = pos;
}
void checkStepTimer(unsigned long lt, unsigned long ct, int axis)
{
    if (lt + AXIS_SPEED[axis] > ct)
    {
        digitalWrite(STEP_PINS[axis], HIGH);
        delayMicroseconds(5);
        digitalWrite(STEP_PINS[axis], LOW);
    }
}
void mov2(int axis1, int pos1, int axis2, int pos2)
{
    writeDirPin(axis1, pos1);
    writeDirPin(axis2, pos2);
    int numOfStepps1 = abs(pos1 - AXIS_POS[axis1]);
    numOfStepps1 = AXIS_STEPS_PER_DEGREE[axis1] * numOfStepps1;
    int numOfStepps2 = abs(pos2 - AXIS_POS[axis2]);
    numOfStepps2 = AXIS_STEPS_PER_DEGREE[axis2] * numOfStepps2;
    unsigned long lt1 = millis();
    unsigned long lt2 = millis();
    unsigned long ct;
    digitalWrite(STEP_PINS[axis1], LOW);
    digitalWrite(STEP_PINS[axis2], LOW);
    while (max(numOfStepps1, numOfStepps2) > 0)
    {
        ct = millis();
        if (numOfStepps1 > 0)
        {
            checkStepTimer(lt1, ct, axis1);
            numOfStepps1--;
            ct = millis();
            lt1 = ct;
        }
        if (numOfStepps2 > 0)
        {
            checkStepTimer(lt2, ct, axis2);
            numOfStepps2--;
            ct = millis();
            lt2 = ct;
        }
    }
}


void mov3(int axis1, int pos1, int axis2, int pos2,int axis3,int pos3)  {
     writeDirPin(axis1, pos1);
    writeDirPin(axis2, pos2);
    writeDirPin(axis3, pos3);
    int numOfStepps1 = abs(pos1 - AXIS_POS[axis1]);
    numOfStepps1 = AXIS_STEPS_PER_DEGREE[axis1] * numOfStepps1;
    int numOfStepps2 = abs(pos2 - AXIS_POS[axis2]);
    numOfStepps2 = AXIS_STEPS_PER_DEGREE[axis2] * numOfStepps2;
    int numOfStepps3 = abs(pos3 - AXIS_POS[axis3]);
    numOfStepps3 = AXIS_STEPS_PER_DEGREE[axis3] * numOfStepps3;
    unsigned long lt1 = millis();
    unsigned long lt2 = millis();
    unsigned long lt3 = millis();
    unsigned long ct;
    digitalWrite(STEP_PINS[axis1], LOW);
    digitalWrite(STEP_PINS[axis2], LOW);
    digitalWrite(STEP_PINS[axis3], LOW);
    while(numOfStepps1 > 0 || numOfStepps3 > 0 ||numOfStepps3 > 0 )
    {
        ct = millis();
        if (numOfStepps1 > 0)
        {
            checkStepTimer(lt1, ct, axis1);
            numOfStepps1--;
            ct = millis();
            lt1 = ct;
        }
        if (numOfStepps2 > 0)
        {
            checkStepTimer(lt2, ct, axis2);
            numOfStepps2--;
            ct = millis();
            lt2 = ct;
        }
        if (numOfStepps3 > 0)
        {
            checkStepTimer(lt3, ct, axis3);
            numOfStepps3--;
            ct = millis();
            lt3 = ct;
        }
    }
}
