#include "servo.h"
#include "stepper.h"
#include <stdlib.h>
#include <Arduino.h>
void procedureInitialize();
void procedureG0(char **commands);
void procedureG1(char **commands);
void procedureG2(char **commands);
void procedureG3(char **commands);
void procedureG4(char **commands);
void procedureG5(char **commands);
void procedureG10(char **commands);
void procedureG11(char **commands);
void procedureM0(char **commands);