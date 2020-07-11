#include "procedures.h"
#include <stdlib.h>
//#include <cstdio>

void procedureG0(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int deg = atoi(commands[2]);
    //printf("proce,dure G0\n");
    //printf("%d\n",axis);
    //printf("%d\n",deg);
    mov0(axis,deg);
}
void procedureG1(char **commands){
    char *temp = commands[1];
    int axis = temp[0]-65;
    int pos = atoi(commands[2]);
    //printf("proce,dure G1\n");
    //printf("%d\n",axis);
    //printf("%d\n",pos);
    mov1(axis,pos);
}
void procedureG2(char **commands){
    char *temp = commands[1];
    int axis1 = temp[0]-65;
    int pos1 = atoi(commands[2]);
    temp = commands[3];
    int axis2 = temp[0]-65;
    int pos2 = atoi(commands[4]);
    //printf("proce,dure G2\n");
    //printf("%d\n",axis1);
    //printf("%d\n",pos1);
    //printf("%d\n",axis2);
    //printf("%d\n",pos2);
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
    //printf("proce,dure G3\n");
    //printf("%d\n",axis1);
    //printf("%d\n",pos1);
    //printf("%d\n",axis2);
    //printf("%d\n",pos2);
    //printf("%d\n",axis3);
    //printf("%d\n",pos3);
    mov3(axis1,pos1,axis2,pos2,axis3,pos3);
}
void procedureG4(char **commands){
    

}
void procedureG5(char **commands){
    
}
void procedureG10(char **commands)  {
    int deg = atoi(commands[2]);
    //printf("%d\n",deg);
    movServo(deg);
}
void procedureG11(char **commands)  {
    int deg = atoi(commands[2]);
    //printf("%d\n",deg);
    movServo(deg);
}