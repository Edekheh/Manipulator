#include "stepper.h"
#include "pins.h"
#define TIMER1_INTERRUPTS_ON TIMSK1 |= (1 << OCIE1A);
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A);
#define NUM_STEPPERS 5
struct stepperInfo
{
  // externally defined parameters
  float acceleration;
  int steps_per_rev;
  float max_revs;
  int degrees;
  float steps_per_deg = (steps_per_rev * max_revs) / degrees;
  int currentPosition = 0;
  volatile unsigned long minStepInterval; // ie. max speed, smaller is faster
  void (*dirFunc)(int);
  void (*stepFunc)();

  // derived parameters
  unsigned int c0;   // step interval for first step, determines acceleration
  long stepPosition; // current position of stepper (total of all movements taken so far)

  // per movement variables (only changed once per movement)
  volatile int dir;                       // current direction of movement, used to keep track of position
  volatile unsigned int totalSteps;       // number of steps requested for current movement
  volatile bool movementDone = false;     // true if the current movement has been completed (used by main program to wait for completion)
  volatile unsigned int rampUpStepCount;  // number of steps taken to reach either max speed, or half-way to the goal (will be zero until this number is known)
  volatile unsigned long estStepsToSpeed; // estimated steps required to reach max speed
  volatile unsigned long estTimeForMove;  // estimated time (interrupt ticks) required to complete movement
  volatile unsigned long rampUpStepTime;
  volatile float speedScale; // used to slow down this motor to make coordinated movement with other motors

  // per iteration variables (potentially changed every interrupt)
  volatile unsigned int n;         // index in acceleration curve, used to calculate next interval
  volatile float d;                // current interval length
  volatile unsigned long di;       // above variable truncated
  volatile unsigned int stepCount; // number of steps completed in current movement
};
volatile stepperInfo steppers[NUM_STEPPERS];

void resetStepperInfo(stepperInfo &si)
{
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

void xStep()
{
  digitalWrite(X_STEP_PIN, HIGH);
  digitalWrite(X_STEP_PIN, LOW);
}
void xDir(int dir)
{
  digitalWrite(X_DIR_PIN, dir);
}

void yStep()
{
  digitalWrite(Y_STEP_PIN, HIGH);
  digitalWrite(Y_STEP_PIN, LOW);
}
void yDir(int dir)
{
  digitalWrite(Y_DIR_PIN, dir);
}

void zStep()
{
  digitalWrite(Z_STEP_PIN, HIGH);
  digitalWrite(Z_STEP_PIN, LOW);
}
void zDir(int dir)
{
  digitalWrite(Z_DIR_PIN, dir);
}

void aStep()
{
  digitalWrite(A_STEP_PIN, HIGH);
  digitalWrite(A_STEP_PIN, LOW);
}
void aDir(int dir)
{
  digitalWrite(A_DIR_PIN, dir);
}

void bStep()
{
  digitalWrite(B_STEP_PIN, HIGH);
  digitalWrite(B_STEP_PIN, LOW);
}
void bDir(int dir)
{
  digitalWrite(B_DIR_PIN, dir);
}

void initializeSteppers()
{
  steppers[0].dirFunc = xDir;
  steppers[0].stepFunc = xStep;
  steppers[0].acceleration = 1000;
  steppers[0].minStepInterval = 50;
  steppers[0].steps_per_rev = 3200;
  steppers[0].max_revs = 5;
  steppers[0].degrees = 180;

  steppers[1].dirFunc = yDir;
  steppers[1].stepFunc = yStep;
  steppers[1].acceleration = 1000;
  steppers[1].minStepInterval = 50;
  steppers[1].steps_per_rev = 25600;
  steppers[1].max_revs = 3.2;
  steppers[1].degrees = 180;

  steppers[2].dirFunc = zDir;
  steppers[2].stepFunc = zStep;
  steppers[2].acceleration = 1000;
  steppers[2].minStepInterval = 50;
  steppers[2].steps_per_rev = 3200;
  steppers[2].max_revs = 9;
  steppers[2].degrees = 180;

  steppers[3].dirFunc = aDir;
  steppers[3].stepFunc = aStep;
  steppers[3].acceleration = 1000;
  steppers[3].minStepInterval = 50;
  steppers[3].steps_per_rev = 3200;
  steppers[3].max_revs = 1;
  steppers[3].degrees = 360;

  steppers[4].dirFunc = bDir;
  steppers[4].stepFunc = bStep;
  steppers[4].acceleration = 1000;
  steppers[4].minStepInterval = 50;
  steppers[4].steps_per_rev = 3200;
  steppers[4].max_revs = 1.8;
  steppers[4].degrees = 180;
}

void resetStepper(volatile stepperInfo &si)
{
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

  float m = ((a * a - 1) / (-2 * a));
  float n = m * m;

  si.estStepsToSpeed = n;
}

volatile byte remainingSteppersFlag = 0;

float getDurationOfAcceleration(volatile stepperInfo &s, unsigned int numSteps)
{
  float d = s.c0;
  float totalDuration = 0;
  for (unsigned int n = 1; n < numSteps; n++)
  {
    d = d - (2 * d) / (4 * n + 1);
    totalDuration += d;
  }
  return totalDuration;
}

void prepareMovement(int whichMotor, long steps)
{
  volatile stepperInfo &si = steppers[whichMotor];
  si.dirFunc(steps < 0 ? HIGH : LOW);
  si.dir = steps > 0 ? 1 : -1;
  si.totalSteps = abs(steps);
  resetStepper(si);

  remainingSteppersFlag |= (1 << whichMotor);

  unsigned long stepsAbs = abs(steps);

  if ((2 * si.estStepsToSpeed) < stepsAbs)
  {
    // there will be a period of time at full speed
    unsigned long stepsAtFullSpeed = stepsAbs - 2 * si.estStepsToSpeed;
    float accelDecelTime = getDurationOfAcceleration(si, si.estStepsToSpeed);
    si.estTimeForMove = 2 * accelDecelTime + stepsAtFullSpeed * si.minStepInterval;
  }
  else
  {
    // will not reach full speed before needing to slow down again
    float accelDecelTime = getDurationOfAcceleration(si, stepsAbs / 2);
    si.estTimeForMove = 2 * accelDecelTime;
  }
}

volatile byte nextStepperFlag = 0;

void setNextInterruptInterval()
{

  bool movementComplete = true;

  unsigned long mind = 999999;
  for (int i = 0; i < NUM_STEPPERS; i++)
  {
    if (((1 << i) & remainingSteppersFlag) && steppers[i].di < mind)
    {
      mind = steppers[i].di;
    }
  }

  nextStepperFlag = 0;
  for (int i = 0; i < NUM_STEPPERS; i++)
  {
    if (!steppers[i].movementDone)
      movementComplete = false;
    if (((1 << i) & remainingSteppersFlag) && steppers[i].di == mind)
      nextStepperFlag |= (1 << i);
  }

  if (remainingSteppersFlag == 0)
  {
    TIMER1_INTERRUPTS_OFF
    OCR1A = 65500;
  }

  OCR1A = mind;
}

ISR(TIMER1_COMPA_vect)
{
  unsigned int tmpCtr = OCR1A;

  OCR1A = 65500;

  for (int i = 0; i < NUM_STEPPERS; i++)
  {

    if (!((1 << i) & remainingSteppersFlag))
      continue;

    if (!(nextStepperFlag & (1 << i)))
    {
      steppers[i].di -= tmpCtr;
      continue;
    }

    volatile stepperInfo &s = steppers[i];

    if (s.stepCount < s.totalSteps)
    {
      s.stepFunc();
      s.stepCount++;
      s.stepPosition += s.dir;
      if (s.stepCount >= s.totalSteps)
      {
        s.movementDone = true;
        remainingSteppersFlag &= ~(1 << i);
      }
    }

    if (s.rampUpStepCount == 0)
    {
      s.n++;
      s.d = s.d - (2 * s.d) / (4 * s.n + 1);
      if (s.d <= s.minStepInterval)
      {
        s.d = s.minStepInterval;
        s.rampUpStepCount = s.stepCount;
      }
      if (s.stepCount >= s.totalSteps / 2)
      {
        s.rampUpStepCount = s.stepCount;
      }
      s.rampUpStepTime += s.d;
    }
    else if (s.stepCount >= s.totalSteps - s.rampUpStepCount)
    {
      s.d = (s.d * (4 * s.n + 1)) / (4 * s.n + 1 - 2);
      s.n--;
    }

    s.di = s.d * s.speedScale; // integer
  }

  setNextInterruptInterval();

  TCNT1 = 0;
}

void adjustSpeedScales()
{
  float maxTime = 0;

  for (int i = 0; i < NUM_STEPPERS; i++)
  {
    if (!((1 << i) & remainingSteppersFlag))
      continue;
    if (steppers[i].estTimeForMove > maxTime)
      maxTime = steppers[i].estTimeForMove;
  }

  if (maxTime != 0)
  {
    for (int i = 0; i < NUM_STEPPERS; i++)
    {
      if (!((1 << i) & remainingSteppersFlag))
        continue;
      steppers[i].speedScale = maxTime / steppers[i].estTimeForMove;
    }
  }
}

void runAndWait()
{
  adjustSpeedScales();
  setNextInterruptInterval();
  TIMER1_INTERRUPTS_ON
  while (remainingSteppersFlag)
    ;
  remainingSteppersFlag = 0;
  nextStepperFlag = 0;
}

int countSteppes(int whichMotor, int desiredPos)
{
  int steppsToDo=(steppers[whichMotor].currentPosition-desiredPos);
  steppsToDo*=steppers[whichMotor].steps_per_deg;
  return steppsToDo;
}

bool checkDeg(int axis, int deg)
{
  //TODO
}

bool checkPos(int axis, int pos)
{
  if(pos>=0 && pos<=steppers[axis].degrees) return true;
  return false;
}

void mov0(int axis, int deg)
{
  if (checkDeg(axis, deg))
  {
    prepareMovement(axis, countSteppes(axis, deg));
    runAndWait();
  }
}

void mov1(int axis, int pos)
{
  if (checkPos(axis, pos))
  {
    prepareMovement(axis, countSteppes(axis, pos));
    runAndWait();
  }
}

void mov2(int axis1, int pos1, int axis2, int pos2)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2))
  {
    prepareMovement(axis1, countSteppes(axis1, pos1));
    prepareMovement(axis2, countSteppes(axis2, pos2));
    runAndWait();
  }
}

void mov3(int axis1, int pos1, int axis2, int pos2, int axis3, int pos3)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2) && checkPos(axis3, pos3) )
    {
      prepareMovement(axis1, countSteppes(axis1, pos1));
      prepareMovement(axis2, countSteppes(axis2, pos2));
      prepareMovement(axis3, countSteppes(axis3, pos3));
      runAndWait();
    }
}
void mov4(int axis1, int pos1, int axis2, int pos2, int axis3, int pos3,int axis4, int pos4)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2) && checkPos(axis3, pos3) && checkPos(axis4, pos4 ))
    {
      prepareMovement(axis1, countSteppes(axis1, pos1));
      prepareMovement(axis2, countSteppes(axis2, pos2));
      prepareMovement(axis3, countSteppes(axis3, pos3));
      prepareMovement(axis4, countSteppes(axis3, pos4));
      runAndWait();
    }
}