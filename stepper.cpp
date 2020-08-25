#include "stepper.h"
#include "pins.h"
#define TIMER1_INTERRUPTS_ON TIMSK1 |= (1 << OCIE1A);
#define TIMER1_INTERRUPTS_OFF TIMSK1 &= ~(1 << OCIE1A);
#define NUM_STEPPERS 5
struct stepperInfo
{
  // externally defined parameters
  float acceleration;
  unsigned long steps_per_deg;
  int currentPosition = 0;
  short maxAngle=180;
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
void changeValues(int axis,int parameter,int value)  {
    Serial.print("Axis nr : ");
    Serial.print(axis);
    Serial.print(" new");
    if(parameter==0)  {
      steppers[axis].acceleration=value;
      //EEPROM.update(&steppers[axis].acceleration,value);
      Serial.print(" acceleration = ");
    }
      
    else if(parameter==1) {
steppers[axis].minStepInterval=value;
      Serial.print(" minStepInterval = ");
    }
      
    else if(parameter==2) {
steppers[axis].steps_per_deg=value;
      Serial.print(" steps per degree = ");
    }
      
Serial.println(value);
  
}
void initializeSteppers()
{
  steppers[0].dirFunc = xDir;
  steppers[0].stepFunc = xStep;
  steppers[0].acceleration = 1000;
  steppers[0].minStepInterval = 50;
  steppers[0].steps_per_deg = (3200 * 1) / 60;
  steppers[0].currentPosition=90;

  steppers[1].dirFunc = yDir;
  steppers[1].stepFunc = yStep;
  steppers[1].acceleration = 1000;
  steppers[1].minStepInterval = 10;
  steppers[1].steps_per_deg = (12800) / 60;
  steppers[1].currentPosition=90;

  steppers[2].dirFunc = zDir;
  steppers[2].stepFunc = zStep;
  steppers[2].acceleration = 1000;
  steppers[2].minStepInterval = 50;
  steppers[2].steps_per_deg = (3200 * 1) / 30;
  steppers[2].currentPosition=90;

  steppers[3].dirFunc = aDir;
  steppers[3].stepFunc = aStep;
  steppers[3].acceleration = 1000;
  steppers[3].minStepInterval = 50;
  steppers[3].steps_per_deg = (3200 * 1) / 360;
  steppers[3].currentPosition=180;
  steppers[3].maxAngle=360;

  steppers[4].dirFunc = bDir;
  steppers[4].stepFunc = bStep;
  steppers[4].acceleration = 1000;
  steppers[4].minStepInterval = 50;
  steppers[4].steps_per_deg = (3200 * 1) / 60;
  steppers[4].currentPosition=90;
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

long countAbsoluteSteppes(int whichMotor, int desiredPos)
{
  Serial.print("Desired pos");
  Serial.println(desiredPos);
  long steppsToDo=(desiredPos-steppers[whichMotor].currentPosition);
  Serial.println(steppsToDo);
  steppsToDo*=steppers[whichMotor].steps_per_deg;
  Serial.println(whichMotor);
  Serial.println(steppsToDo);
  Serial.println("^^^Angle ^^Axis number ^steps number");
  if(steppsToDo==0)steppsToDo=48;
  return steppsToDo;
}
long countIncrementalSteppes(int whichMotor,int desiredDeg)  {
  Serial.print("Moving ");
  Serial.print(whichMotor);
  Serial.print(" axis for ");
  Serial.print(desiredDeg);
  Serial.println("degrees");
  long steppsToDo=(desiredDeg);
  steppsToDo*=steppers[whichMotor].steps_per_deg;
  steppers[whichMotor].currentPosition+=desiredDeg;
  Serial.print("After ");
  Serial.print(steppsToDo);
  Serial.print(" steppes axis ");
  Serial.print(whichMotor);
  Serial.print(" new position is");
  Serial.println(steppers[whichMotor].currentPosition);
  if(steppsToDo==0)steppsToDo=48;
  return steppsToDo;
}

bool checkDeg(int axis, int deg)
{
  if(steppers[axis].currentPosition+deg>=0 && steppers[axis].currentPosition+deg<=steppers[axis].maxAngle)  {
    Serial.println("DEG OK");
    return true;
    }
  return false;
}

bool checkPos(int axis, int pos)
{
  if(pos>=0 && pos<=steppers[axis].maxAngle)  {
  Serial.println("CHECK POS OK");
  return true;
  } 
  return false;
}
//incremental move
void mov0(int axis, int deg)
{
  if (checkDeg(axis, deg))
  {
    prepareMovement(axis, countIncrementalSteppes(axis, deg));
    steppers[axis].currentPosition+=deg;
    runAndWait();
  }
}

//absolute move
//working
void mov1(int axis, int pos)
{
  if (checkPos(axis, pos))
  {
    long numOfStepps=countAbsoluteSteppes(axis, pos);
    prepareMovement(axis,numOfStepps );
    runAndWait();
    steppers[axis].currentPosition=pos;
  }
}

void mov2(int axis1, int pos1, int axis2, int pos2)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2))
  {
    prepareMovement(axis1, countAbsoluteSteppes(axis1, pos1));
    prepareMovement(axis2, countAbsoluteSteppes(axis2, pos2));
    runAndWait();
    steppers[axis1].currentPosition=pos1;
    steppers[axis2].currentPosition=pos2;
  }
}

void mov3(int axis1, int pos1, int axis2, int pos2, int axis3, int pos3)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2) && checkPos(axis3, pos3) )
    {
      prepareMovement(axis1, countAbsoluteSteppes(axis1, pos1));
      prepareMovement(axis2, countAbsoluteSteppes(axis2, pos2));
      prepareMovement(axis3, countAbsoluteSteppes(axis3, pos3));
      runAndWait();
      steppers[axis1].currentPosition=pos1;
      steppers[axis2].currentPosition=pos2;
      steppers[axis3].currentPosition=pos3;
    }
}
void mov4(int axis1, int pos1, int axis2, int pos2, int axis3, int pos3,int axis4, int pos4)
{
  if (checkPos(axis1, pos1) && checkPos(axis2, pos2) && checkPos(axis3, pos3) && checkPos(axis4, pos4 ))
    {
      prepareMovement(axis1, countAbsoluteSteppes(axis1, pos1));
      prepareMovement(axis2, countAbsoluteSteppes(axis2, pos2));
      prepareMovement(axis3, countAbsoluteSteppes(axis3, pos3));
      prepareMovement(axis4, countAbsoluteSteppes(axis3, pos4));
      runAndWait();
      steppers[axis1].currentPosition=pos1;
      steppers[axis2].currentPosition=pos2;
      steppers[axis3].currentPosition=pos3;
      steppers[axis4].currentPosition=pos4;
    }
}