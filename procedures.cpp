#include "procedures.h"

void procedureG0(char **commands){
    char *tempData = commands[1];
    int axis = tempData.charAt(0)-65;
    int deg = atoi(tempData);
    mov0(int axis, int deg);
}
void procedureG1(char **commands){
    char *tempData = commands[1];
    int axis = tempData.charAt(0)-65;
    int pos = atoi(tempData);
    mov1(int axis, int pos);
}
void procedureG2(char **commands){
    char *tempData = commands[1];
    int axis1 = tempData.charAt(0)-65;
    int pos1 = atoi(tempData);
    tempData = commands[2];
    int axis2 = tempData.charAt(0)-65;
    int pos2 = atoi(tempData);
    mov2(int axis1,int pos1,int axis2,int pos2);
}
void procedureG3(char **commands){
    char *tempData = commands[1];
    int axis1 = tempData.charAt(0)-65;
    int pos1 = atoi(tempData);
    mov3(int axis1, int pos1, int axis2, int pos2,int axis3,int pos3);
}
void procedureG4(char **commands){
    

}
void procedureG5(char **commands){
    
}
void procedureG10(char **commands)  {
    char *tempData = commands[1];
    int deg = atoi(tempData);
    movServo(int deg);
}
void procedureG11(char **commands)  {
    char *tempData = commands[1];
    int deg = atoi(tempData);
    movServo(int deg);
}