#include "stepper.h"
#include "states.h"
#include "servo.h"
String SERIALPORTVALUE = "";
//axis one (X) -> 0
//axis two (Y) -> 1
//axis three (Z) -> 2
//axis four (E0) -> 3
//axis five (E1) -> 4

void printSetup()
{
  Serial.println("Moving robotic arm by pseudoGCode");
  Serial.println("G1 - moving command, A B C D E F - axis names, degree from 000 to 180");
  Serial.println("Example : G1 A32 ;");
  Serial.println("^^^Moving A axis by 32 degrees^^^");
}
bool spaceSign(char sign)
{
  if (sign == ' ')
    return true;
  else
    return false;
}
bool endSign(char sign)
{
  
}
int translateAxis(char sign)
{
  int tempAxis;
  if (sign == 'A')
    tempAxis = 0; //translating 1digit from string to axis name
  else if (sign == 'B')
    tempAxis = 1;
  else if (sign == 'C')
    tempAxis = 2;
  else if (sign == 'D')
    tempAxis = 3;
  else if (sign == 'E')
    tempAxis = 4;
  else if (sign == 'F')
    tempAxis = 5;
  else
    tempAxis = 412; //error axis not found
  return tempAxis;
}
bool digitSign(char sign) {
if (sign >47 && sign<58)
    return true;
  else
    return false;
}
bool axisNameSign(char sign)  {
  if (sign >64 && sign<71)
    return true;
  else
    return false;
}
bool checkAxisTranslation(int axis, int pos)
{
  if (axis < 300 && AXIS_DEGREES[axis] >= pos && pos >= 0)
    return true;
  else
    return false;
}
void printMovingByCommand(int axis, int pos, char a)
{
    Serial.print("Moving ");
    Serial.print(a); //name of axis
    Serial.print(" axis by ");
    int t1 = abs(AXIS_POS[axis] - pos); //how much degrees
    Serial.print(t1);
    Serial.println(" degree");
    Serial.print("Axis position : ");
    movAxisDegree(axis, pos);
    Serial.println(AXIS_POS[axis]);
}

void translateString(String str)  {
  int num=str.length();
  int counter=0;
  int tempAxis=0;
  int tempPos=0;
  if(str[counter]=='G') {
    counter++;
    if(str[counter]=='1' && str[counter+1]!='0' && str[counter+1]!='1') {
      counter++;//space
      while(counter<=num) {
        tempAxis=0;
        tempPos=0;
        counter++;
        tempAxis=str[counter]-65;
        counter++;
        tempPos+=(str[counter]-48)*100;
        counter++;
        tempPos+=(str[counter]-48)*10;
        counter++;
        tempPos+=(str[counter]-48)*1;
        counter++;
        if(tempPos>=0 && tempPos<=AXIS_DEGREES[tempAxis]) {
          char tempA=str[counter-4];
            printMovingByCommand(tempAxis,tempPos,tempA);
        }
      }
      Serial.println("OPERATIONAL");

  }
  else if(str[counter]=='1' && str[counter+1]=='0')  {//close gripper
  counter++;
  movServo(Servo1,180);
  
  }
  else if(str[counter]=='1' && str[counter+1]=='1')  {//open gripper
    counter++;
      movServo(Servo1,0);
  }
  }
}