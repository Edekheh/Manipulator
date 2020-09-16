#include "procedures.h"
#include <stdlib.h>
//#include <cstdio>

void procedureInitialize()   {
    initializeSteppers();
    Serial.println("Steppers initialized");
}
void procedureG0(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int deg = atoi(commands[2]);
    Serial.println("Procedure G0");
    Serial.println(axis);
    Serial.println(deg);
    mov0(axis,deg);
}
void procedureM0(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int parameter = atoi(commands[2]);
    int value = atoi(commands[3]);
    Serial.println("Procedure M0");
    Serial.println(axis);
    Serial.println(parameter);
    Serial.println(value);
    changeValues(axis,parameter,value);
}
void printMovingProcedure(int axis,int pos)  {
    Serial.print("Moving Axis ");
    Serial.print(axis);
    Serial.print(" on pos ");
    Serial.println(pos);
}
void procedureG1(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int pos = atoi(commands[2]);
    printMovingProcedure(axis,pos);
    mov1(axis,pos);
}
void procedureG2(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    printMovingProcedure(axis1,pos1);
    printMovingProcedure(axis2,pos2);
    mov2(axis1,pos1,axis2,pos2);
}
void procedureG3(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    temp = commands[5];
    int axis3 = temp[0]-65;
    int pos3 = atoi(commands[6]);
    printMovingProcedure(axis1,pos1);
    printMovingProcedure(axis2,pos2);
    printMovingProcedure(axis3,pos3);
    mov3(axis1,pos1,axis2,pos2,axis3,pos3);
}
void procedureG4(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    temp = commands[5];
    int axis3 = temp[0]-65;
    int pos3 = atoi(commands[6]);
    temp = commands[7];
    int axis4 = temp[0]-65;
    int pos4 = atoi(commands[8]);
    printMovingProcedure(axis1,pos1);
    printMovingProcedure(axis2,pos2);
    printMovingProcedure(axis3,pos3);
    printMovingProcedure(axis4,pos4);
    mov4(axis1,pos1,axis2,pos2,axis3,pos3,axis4,pos4);
}
void procedureG5(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    temp = commands[5];
    int axis3 = temp[0]-65;
    int pos3 = atoi(commands[6]);
    temp = commands[7];
    int axis4 = temp[0]-65;
    int pos4 = atoi(commands[8]);
    temp = commands[9];
    int axis5= temp[0]-65;
    int pos5 = atoi(commands[10]);
    printMovingProcedure(axis1,pos1);
    printMovingProcedure(axis2,pos2);
    printMovingProcedure(axis3,pos3);
    printMovingProcedure(axis4,pos4);
    printMovingProcedure(axis5,pos5);
    mov5(axis1,pos1,axis2,pos2,axis3,pos3,axis4,pos4,axis5,pos5);
}
void procedureG10()  {
    openServoG10();
}
void procedureG11()  {
    closeServoG11();
}