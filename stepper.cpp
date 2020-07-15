#include "stepper.h"
#include "pins.h"
#define TIMER1_INTERRUPTS_ON    TIMSK1 |=  (1 << OCIE1A);
#define TIMER1_INTERRUPTS_OFF   TIMSK1 &= ~(1 << OCIE1A);
#define NUM_STEPPERS 5
#define X_STEP_HIGH             PORTF |=  0b00000001;
#define X_STEP_LOW              PORTF &= ~0b00000001;

#define Y_STEP_HIGH             PORTF |=  0b01000000;
#define Y_STEP_LOW              PORTF &= ~0b01000000;

#define Z_STEP_HIGH             PORTL |=  0b00001000;
#define Z_STEP_LOW              PORTL &= ~0b00001000;

#define A_STEP_HIGH             PORTA |=  0b00010000;
#define A_STEP_LOW              PORTA &= ~0b00010000;

#define B_STEP_HIGH             PORTC |=  0b00000010;
#define B_STEP_LOW              PORTC &= ~0b00000010;


struct stepperInfo {
  // externally defined parameters
  float acceleration;
  volatile unsigned long minStepInterval; // ie. max speed, smaller is faster
  void (*dirFunc)(int);
  void (*stepFunc)();

  // derived parameters
  unsigned int c0;                // step interval for first step, determines acceleration
  long stepPosition;              // current position of stepper (total of all movements taken so far)

  // per movement variables (only changed once per movement)
  volatile int dir;                        // current direction of movement, used to keep track of position
  volatile unsigned int totalSteps;        // number of steps requested for current movement
  volatile bool movementDone = false;      // true if the current movement has been completed (used by main program to wait for completion)
  volatile unsigned int rampUpStepCount;   // number of steps taken to reach either max speed, or half-way to the goal (will be zero until this number is known)
  volatile unsigned long estStepsToSpeed;  // estimated steps required to reach max speed
  volatile unsigned long estTimeForMove;   // estimated time (interrupt ticks) required to complete movement
  volatile unsigned long rampUpStepTime;
  volatile float speedScale;               // used to slow down this motor to make coordinated movement with other motors

  // per iteration variables (potentially changed every interrupt)
  volatile unsigned int n;                 // index in acceleration curve, used to calculate next interval
  volatile float d;                        // current interval length
  volatile unsigned long di;               // above variable truncated
  volatile unsigned int stepCount;         // number of steps completed in current movement
};
volatile stepperInfo steppers[NUM_STEPPERS];

void resetStepperInfo( stepperInfo& si ) {
  si.n = 0;
  si.d = 0;
  si.di = 0;
  si.stepCount = 0;
  si.rampUpStepCount = 0;
  si.rampUpStepTime = 0;
  si.totalSteps = 0;
  si.stepPosition = 0;
  si.movementDone = false;
}

void xStep() {
  X_STEP_HIGH
  X_STEP_LOW
}
void xDir(int dir) {
  digitalWrite(X_DIR_PIN, dir);
}

void yStep() {
  Y_STEP_HIGH
  Y_STEP_LOW
}
void yDir(int dir) {
  digitalWrite(Y_DIR_PIN, dir);
}

void zStep() {
  Z_STEP_HIGH
  Z_STEP_LOW
}
void zDir(int dir) {
  digitalWrite(Z_DIR_PIN, dir);
}

void aStep() {
  A_STEP_HIGH
  A_STEP_LOW
}
void aDir(int dir) {
  digitalWrite(A_DIR_PIN, dir);
}

void bStep() {
  B_STEP_HIGH
  B_STEP_LOW
}
void bDir(int dir) {
  digitalWrite(B_DIR_PIN, dir);
}

void initializeSteppers()   {
    steppers[0].dirFunc = xDir;
  steppers[0].stepFunc = xStep;
  steppers[0].acceleration = 1000;
  steppers[0].minStepInterval = 50;

  steppers[1].dirFunc = yDir;
  steppers[1].stepFunc = yStep;
  steppers[1].acceleration = 1000;
  steppers[1].minStepInterval = 50;

  steppers[2].dirFunc = zDir;
  steppers[2].stepFunc = zStep;
  steppers[2].acceleration = 1000;
  steppers[2].minStepInterval = 50;

  steppers[3].dirFunc = aDir;
  steppers[3].stepFunc = aStep;
  steppers[3].acceleration = 1000;
  steppers[3].minStepInterval = 50;

  steppers[4].dirFunc = bDir;
  steppers[4].stepFunc = bStep;
  steppers[4].acceleration = 1000;
  steppers[4].minStepInterval = 50;
}

void resetStepper(volatile stepperInfo& si) {
  si.c0 = si.acceleration;
  si.d = si.c0;
  si.di = si.d;
  si.stepCount = 0;
  si.n = 0;
  si.rampUpStepCount = 0;
  si.movementDone = false;
  si.speedScale = 1;

  float a = si.minStepInterval / (float)si.c0;
  a *= 0.676;

  float m = ((a*a - 1) / (-2 * a));
  float n = m * m;

  si.estStepsToSpeed = n;
}


volatile byte remainingSteppersFlag = 0;

float getDurationOfAcceleration(volatile stepperInfo& s, unsigned int numSteps) {
  float d = s.c0;
  float totalDuration = 0;
  for (unsigned int n = 1; n < numSteps; n++) {
    d = d - (2 * d) / (4 * n + 1);
    totalDuration += d;
  }
  return totalDuration;
}

void prepareMovement(int whichMotor, long steps) {
  volatile stepperInfo& si = steppers[whichMotor];
  si.dirFunc( steps < 0 ? HIGH : LOW );
  si.dir = steps > 0 ? 1 : -1;
  si.totalSteps = abs(steps);
  resetStepper(si);
  
  remainingSteppersFlag |= (1 << whichMotor);

  unsigned long stepsAbs = abs(steps);

  if ( (2 * si.estStepsToSpeed) < stepsAbs ) {
    // there will be a period of time at full speed
    unsigned long stepsAtFullSpeed = stepsAbs - 2 * si.estStepsToSpeed;
    float accelDecelTime = getDurationOfAcceleration(si, si.estStepsToSpeed);
    si.estTimeForMove = 2 * accelDecelTime + stepsAtFullSpeed * si.minStepInterval;
  }
  else {
    // will not reach full speed before needing to slow down again
    float accelDecelTime = getDurationOfAcceleration( si, stepsAbs / 2 );
    si.estTimeForMove = 2 * accelDecelTime;
  }
}

volatile byte nextStepperFlag = 0;

void setNextInterruptInterval() {

  bool movementComplete = true;

  unsigned long mind = 999999;
  for (int i = 0; i < NUM_STEPPERS; i++) {
    if ( ((1 << i) & remainingSteppersFlag) && steppers[i].di < mind ) {
      mind = steppers[i].di;
    }
  }

  nextStepperFlag = 0;
  for (int i = 0; i < NUM_STEPPERS; i++) {
    if ( ! steppers[i].movementDone )
      movementComplete = false;
    if ( ((1 << i) & remainingSteppersFlag) && steppers[i].di == mind )
      nextStepperFlag |= (1 << i);
  }

  if ( remainingSteppersFlag == 0 ) {
    TIMER1_INTERRUPTS_OFF
    OCR1A = 65500;
  }

  OCR1A = mind;
}

ISR(TIMER1_COMPA_vect)
{
  unsigned int tmpCtr = OCR1A;

  OCR1A = 65500;

  for (int i = 0; i < NUM_STEPPERS; i++) {

    if ( ! ((1 << i) & remainingSteppersFlag) )
      continue;

    if ( ! (nextStepperFlag & (1 << i)) ) {
      steppers[i].di -= tmpCtr;
      continue;
    }

    volatile stepperInfo& s = steppers[i];

    if ( s.stepCount < s.totalSteps ) {
      s.stepFunc();
      s.stepCount++;
      s.stepPosition += s.dir;
      if ( s.stepCount >= s.totalSteps ) {
        s.movementDone = true;
        remainingSteppersFlag &= ~(1 << i);
      }
    }

    if ( s.rampUpStepCount == 0 ) {
      s.n++;
      s.d = s.d - (2 * s.d) / (4 * s.n + 1);
      if ( s.d <= s.minStepInterval ) {
        s.d = s.minStepInterval;
        s.rampUpStepCount = s.stepCount;
      }
      if ( s.stepCount >= s.totalSteps / 2 ) {
        s.rampUpStepCount = s.stepCount;
      }
      s.rampUpStepTime += s.d;
    }
    else if ( s.stepCount >= s.totalSteps - s.rampUpStepCount ) {
      s.d = (s.d * (4 * s.n + 1)) / (4 * s.n + 1 - 2);
      s.n--;
    }

    s.di = s.d * s.speedScale; // integer
  }

  setNextInterruptInterval();

  TCNT1  = 0;
}


void adjustSpeedScales() {
  float maxTime = 0;
  
  for (int i = 0; i < NUM_STEPPERS; i++) {
    if ( ! ((1 << i) & remainingSteppersFlag) )
      continue;
    if ( steppers[i].estTimeForMove > maxTime )
      maxTime = steppers[i].estTimeForMove;
  }

  if ( maxTime != 0 ) {
    for (int i = 0; i < NUM_STEPPERS; i++) {
      if ( ! ( (1 << i) & remainingSteppersFlag) )
        continue;
      steppers[i].speedScale = maxTime / steppers[i].estTimeForMove;
    }
  }
}

void runAndWait() {
  adjustSpeedScales();
  setNextInterruptInterval();
  TIMER1_INTERRUPTS_ON
  while ( remainingSteppersFlag );
  remainingSteppersFlag = 0;
  nextStepperFlag = 0;
}







//stepper motor controlls
int STEP_PINS[5] = {X_STEP_PIN, Y_STEP_PIN, Z_STEP_PIN, A_STEP_PIN, B_STEP_PIN};
int DIR_PINS[5] = {X_DIR_PIN, Y_DIR_PIN, Z_DIR_PIN, A_DIR_PIN, B_DIR_PIN};
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
        delayMicroseconds(100);
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
    unsigned long lt1 = micros();
    unsigned long lt2 = micros();
    unsigned long ct;
    digitalWrite(STEP_PINS[axis1], LOW);
    digitalWrite(STEP_PINS[axis2], LOW);
    while (max(numOfStepps1, numOfStepps2) > 0)
    {
        ct = micros();
        if (numOfStepps1 > 0)
        {
            checkStepTimer(lt1, ct, axis1);
            numOfStepps1--;
            ct = micros();
            lt1 = ct;
        }
        if (numOfStepps2 > 0)
        {
            checkStepTimer(lt2, ct, axis2);
            numOfStepps2--;
            ct = micros();
            lt2 = ct;
        }
    }
    AXIS_POS[axis1] = pos1;
    AXIS_POS[axis2] = pos2;
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
    unsigned long lt1 = micros();
    unsigned long lt2 = micros();
    unsigned long lt3 = micros();
    unsigned long ct;
    digitalWrite(STEP_PINS[axis1], LOW);
    digitalWrite(STEP_PINS[axis2], LOW);
    digitalWrite(STEP_PINS[axis3], LOW);
    while(numOfStepps1 > 0 || numOfStepps3 > 0 ||numOfStepps3 > 0 )
    {
        ct = micros();
        if (numOfStepps1 > 0)
        {
            checkStepTimer(lt1, ct, axis1);
            numOfStepps1--;
            ct = micros();
            lt1 = ct;
        }
        if (numOfStepps2 > 0)
        {
            checkStepTimer(lt2, ct, axis2);
            numOfStepps2--;
            ct = micros();
            lt2 = ct;
        }
        if (numOfStepps3 > 0)
        {
            checkStepTimer(lt3, ct, axis3);
            numOfStepps3--;
            ct = micros();
            lt3 = ct;
        }
    }
}
