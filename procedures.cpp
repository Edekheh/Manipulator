#include "procedures.h"
#include <stdlib.h>
//#include <cstdio>

void procedureG0(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int deg = atoi(commands[2]);
    Serial.println("dure G0\n");
    Serial.println(axis);
    Serial.println(deg);
    mov0(axis,deg);
}
void procedureG1(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int pos = atoi(commands[2]);
    Serial.println("Procedure G1");
    Serial.println(axis);
    Serial.println(pos);
    mov1(axis,pos);
}
void procedureG2(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    Serial.println("dure G2\n");
    Serial.println(axis1);
    Serial.println(pos1);
    Serial.println(axis2);
    Serial.println(pos2);
    mov2(axis1,pos1,axis2,pos2);
}
void procedureG3(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[1];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    temp = commands[1];
    int axis3 = temp[0]-65;
    int pos3 = atoi(commands[6]);
    Serial.println("dure G3\n");
    Serial.println(axis1);
    Serial.println(pos1);
    Serial.println(axis2);
    Serial.println(pos2);
    Serial.println(axis3);
    Serial.println(pos3);
    mov3(axis1,pos1,axis2,pos2,axis3,pos3);
}
void procedureG4(char **commands){
    

}
void procedureG5(char **commands){
    
}
void procedureG10(char **commands)  {
    int deg = atoi(commands[2]);
    Serial.println(deg);
    movServo(deg);
}
void procedureG11(char **commands)  {
    int deg = atoi(commands[2]);
    Serial.println(deg);
    movServo(deg);
}