#include<stdlib.h>
#include<cstdio>
#include"../interpreter.h"
int main(int argc, char *argv[]){
    while(1){
    char testValues[]="G1 A 90";
    printf("%s",testValues);
    int a = interpreter(testValues);
    printf("%d",a);
    char atestValues[]="G2 A 90 B 90";
    a = interpreter(atestValues);
    printf("%d",a);
    char btestValues[]="G2 C 90 D 90";
    a = interpreter(btestValues);
    printf("%d",a);
    }
}