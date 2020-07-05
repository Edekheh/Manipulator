#include "procedures.h"
#include <stdlib.h>

void procedureG0(char **commands){
    char *tempData = commands[1];
    int axis = tempData[0]-65;
    int deg = atoi(tempData);
    mov0(axis,deg);
}
void procedureG1(char **commands){
    char *tempData = commands[1];
    int axis = tempData[0]-65;
    int pos = atoi(tempData);
    mov1(axis,pos);
}
void procedureG2(char **commands){
    char *tempData = commands[1];
    int axis1 = tempData[0]-65;
    int pos1 = atoi(tempData);
    tempData = commands[2];
    int axis2 = tempData[0]-65;
    int pos2 = atoi(tempData);
    mov2(axis1,pos1,axis2,pos2);
}
void procedureG3(char **commands){
    char *tempData = commands[1];
    int axis1 = tempData[0]-65;
    int pos1 = atoi(tempData);
    tempData = commands[2];
    int axis2 = tempData[0]-65;
    int pos2 = atoi(tempData);
    tempData = commands[3];
    int axis3 = tempData[0]-65;
    int pos3 = atoi(tempData);
    mov3(axis1,pos1,axis2,pos2,axis3,pos3);
}
void procedureG4(char **commands){
    

}
void procedureG5(char **commands){
    
}
void procedureG10(char **commands)  {
    char *tempData = commands[1];
    int deg = atoi(tempData);
    movServo(deg);
}
void procedureG11(char **commands)  {
    char *tempData = commands[1];
    int deg = atoi(tempData);
    movServo(deg);
}