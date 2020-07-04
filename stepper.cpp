#include "stepper.h"
#include "pins.h"
//stepper motor controlls
int STEP_PINS[5]={X_STEP_PIN,Y_STEP_PIN,Z_STEP_PIN,E0_STEP_PIN,E1_STEP_PIN};
int DIR_PINS[5]={X_DIR_PIN,Y_DIR_PIN,Z_DIR_PIN,E0_DIR_PIN,E1_DIR_PIN};
int ENABLE_PINS[5];
int AXIS_STEPS_PER_REV[5] = {3200,3200,3200,3200,3200};//microstepps
float AXIS_MAX_REV[5] = {5, 3.2, 9, 1, 1.8};
int AXIS_POS[5] = {90, 90, 90, 180, 90};
int AXIS_DEGREES[5] = {180, 180, 180, 360, 180};
int AXIS_SPEED[5] = {400, 300, 200, 300, 500};
bool AXIS_DIRECTION[5]={1,0,0,0,1};//1 for inverse logic
float AXIS_STEPS_PER_DEGREE[5]={(AXIS_STEPS_PER_REV[0] * AXIS_MAX_REV[0]) / AXIS_DEGREES[0],
(AXIS_STEPS_PER_REV[1] * AXIS_MAX_REV[1]) / AXIS_DEGREES[1],
(AXIS_STEPS_PER_REV[2] * AXIS_MAX_REV[2]) / AXIS_DEGREES[2],
(AXIS_STEPS_PER_REV[3] * AXIS_MAX_REV[3]) / AXIS_DEGREES[3],
(AXIS_STEPS_PER_REV[4] * AXIS_MAX_REV[4]) / AXIS_DEGREES[4]
};
void writeDirPin(int axis,int pos)  {
if(pos>AXIS_POS[axis])  {
        digitalWrite(DIR_PINS[axis], AXIS_DIRECTION[axis]);
    }
    else if(pos<AXIS_POS[axis]) {
        digitalWrite(DIR_PINS[axis], !AXIS_DIRECTION[axis]);
    }
}
void mov1(int axis,int pos) {
    int tempPos=abs(pos-AXIS_POS[axis]);
    writeDirPin(axis,pos);
    for (int i = 0; i <  AXIS_STEPS_PER_DEGREE[axis] * tempPos; i++) {
      digitalWrite(STEP_PINS[axis], HIGH);
      delayMicroseconds(AXIS_SPEED[axis]);
      digitalWrite(STEP_PINS[axis], LOW);
      delayMicroseconds(AXIS_SPEED[axis]);
    }
    AXIS_POS[axis]=pos;
}
