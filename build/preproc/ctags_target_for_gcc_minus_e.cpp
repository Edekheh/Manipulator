# 1 "c:\\Users\\dawid\\Desktop\\Manipulator\\Software\\first\\first.ino"
# 2 "c:\\Users\\dawid\\Desktop\\Manipulator\\Software\\first\\first.ino" 2



float AXIS_MAX_REV[5] = {5, 3.2, 1, 1, 2};
int AXIS_POS[5] = {0, 0, 0, 0, 0};
int AXIS_DEGREES[5] = {180, 180, 180, 180, 200};
int AXIS_SPEED[5] = {100, 300, 500, 500, 500};
//axis one (X) -> 0
//axis two (Y) -> 1
//axis three (Z) -> 2
//axis four (E0) -> 3
//axis five (E1) -> 4
String SERIALPORTVALUE = "";
void movByCommand(String str);
void setup() {
  definePinMode();
  Serial.begin(9600);
  Serial.println("Moving robotic arm by pseudoGCode");
  Serial.println("G1 - moving command, A B C D E F - axis names, degree from 000 to 180");
  Serial.println("Example : G1 A032");
  Serial.println("^^^Moving A axis by 32 degrees^^^");
  Serial.println("State: Operational");
}

void loop() {
  while (Serial.available()) {
    SERIALPORTVALUE = Serial.readString();
  }
  if (SERIALPORTVALUE.length() > 2) {
    Serial.println("move:");
    movByCommand(SERIALPORTVALUE);
    SERIALPORTVALUE = "";
  }
}
void movAxisDegree(int axis, int pos) { //pos from 0 to axis degree
  float stepsPerDegree = (3200 * AXIS_MAX_REV[axis]) / AXIS_DEGREES[axis];
  if (pos > AXIS_POS[axis]) {
    int tempPos = pos - AXIS_POS[axis];
    digitalWrite(DIR_PINS[axis], 0x0);
    for (int i = 0; i < stepsPerDegree * tempPos; i++) {
      digitalWrite(STEP_PINS[axis], 0x1);
      delayMicroseconds(AXIS_SPEED[axis]);
      digitalWrite(STEP_PINS[axis], 0x0);
      delayMicroseconds(AXIS_SPEED[axis]);
    }
  }
  else if (pos < AXIS_POS[axis]) {
    digitalWrite(DIR_PINS[axis], 0x1);
    int temp1 = AXIS_POS[axis] - pos;
    for (int i = 0; i < stepsPerDegree * temp1; i++) {
      digitalWrite(STEP_PINS[axis], 0x1);
      delayMicroseconds(AXIS_SPEED[axis]);
      digitalWrite(STEP_PINS[axis], 0x0);
      delayMicroseconds(AXIS_SPEED[axis]);
    }
  }
  AXIS_POS[axis] = pos;
}
bool spaceSign(char sign) {
  if (sign == ' ')return true;
  else return false;
}
int translateAxis(char sign) {
  int tempAxis;
  if (sign == 'A') tempAxis = 0; //translating 1digit from string to axis name
  else if (sign == 'B') tempAxis = 1;
  else if (sign == 'C') tempAxis = 2;
  else if (sign == 'D') tempAxis = 3;
  else if (sign == 'E') tempAxis = 4;
  return tempAxis;
}
void printMovingByCommand(int axis, int pos, char a) {
  Serial.print("Moving ");
  Serial.print(a);//name of axis
  Serial.print(" axis by ");
  int t1 = ((AXIS_POS[axis] - pos)>0?(AXIS_POS[axis] - pos):-(AXIS_POS[axis] - pos)); //how much degrees
  Serial.print(t1);
  Serial.println(" degree");
  Serial.print("Axis position : ");
  movAxisDegree(axis, pos);
  Serial.println(AXIS_POS[axis]);
  Serial.println("State : Operational");

}
void movByCommand(String str) {//moving robotic arm by command
  if (str[0] == 'G') {
    if (str[1] == '1') {
      int counter = 2;
      while (counter-2<str.length()) {
        int tempAxis = 0;
        int tempPos = 999;
        char tempChar = 0;
        int tempCounter = 0;
        int tab[3] = {0, 0, 0};
        int k = 0;
        if (str[counter + 1] == ';')break;
        else if (spaceSign(str[counter])) {
          if (str[counter + 1] == ';')break;
          counter++;//pointer on axis name
          tempChar = str[counter];
          tempAxis = translateAxis(tempChar);
          while (1) {
            counter++;//pointer on digit
            if (spaceSign(str[counter])) break;
            tab[k] = str[counter] - 48;
            tempCounter++;
            k++;
            if (k == 4)break; //error
          }//counter wskazuje na spacje
          if (tempCounter == 0)break;
          else if (tempCounter == 1)tempPos = tab[0] * 1;
          else if (tempCounter == 2)tempPos = tab[0] * 10 + tab[1] * 1;
          else if (tempCounter == 3)tempPos = tab[0] * 100 + tab[1] * 10 + tab[2] * 1;
          if (AXIS_DEGREES[tempAxis] >= tempPos && tempPos >= 0) { //if read degree is okay
            if (counter == str.length())break;
            printMovingByCommand(tempAxis, tempPos, tempChar);
          }
          else {
            Serial.println("State : Error");
            break;
          }
          if (str[counter + 1] == ';')break;
        }
      }
    }
  }
}
