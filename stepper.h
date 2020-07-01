//stepper motor controlls
int AXIS_STEPS_PER_REV[5] = {3200,3200,3200,3200,3200};//microstepps
float AXIS_MAX_REV[5] = {5, 3.2, 9, 1, 1.8};
int AXIS_POS[5] = {90, 90, 90, 180, 90};
int AXIS_DEGREES[5] = {180, 180, 180, 360, 180};
int AXIS_SPEED[5] = {400, 300, 200, 300, 500};
bool AXIS_DIRECTION[5]={1,0,0,0,1};//1 for inverse logic



void movAxisDegree(int axis, int pos) { //pos from 0 to axis degree
  float stepsPerDegree = (AXIS_STEPS_PER_REV[axis] * AXIS_MAX_REV[axis]) / AXIS_DEGREES[axis];
  if (pos > AXIS_POS[axis])  {
    int tempPos = pos - AXIS_POS[axis];
    digitalWrite(DIR_PINS[axis], AXIS_DIRECTION[axis]);
    for (int i = 0; i < stepsPerDegree * tempPos; i++) {
      digitalWrite(STEP_PINS[axis], HIGH);
      delayMicroseconds(AXIS_SPEED[axis]);
      digitalWrite(STEP_PINS[axis], LOW);
      delayMicroseconds(AXIS_SPEED[axis]);
    }
  }
  else if (pos < AXIS_POS[axis]) {
    digitalWrite(DIR_PINS[axis], !AXIS_DIRECTION[axis]);
    int temp1 = AXIS_POS[axis] - pos;
    for (int i = 0; i < stepsPerDegree * temp1; i++) {
      digitalWrite(STEP_PINS[axis], HIGH);
      delayMicroseconds(AXIS_SPEED[axis]);
      digitalWrite(STEP_PINS[axis], LOW);
      delayMicroseconds(AXIS_SPEED[axis]);
    }
  }
  AXIS_POS[axis] = pos;
}