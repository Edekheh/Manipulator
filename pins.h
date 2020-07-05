#include <Arduino.h>
#define X_STEP_PIN 54
#define X_DIR_PIN 55
#define X_ENABLE_PIN 38
#define Y_STEP_PIN 60
#define Y_DIR_PIN 61
#define Y_ENABLE_PIN 56
#define Z_STEP_PIN 46
#define Z_DIR_PIN 48
#define Z_ENABLE_PIN 62
#define E0_STEP_PIN 26
#define E0_DIR_PIN 28
#define E0_ENABLE_PIN 24
#define E1_STEP_PIN 36
#define E1_DIR_PIN 34
#define E1_ENABLE_PIN 30
#define X_MIN_PIN 3
#define X_MAX_PIN 2
#define Y_MIN_PIN 14
#define Y_MAX_PIN 15
#define Z_MIN_PIN 18
#define Z_MAX_PIN 19
#define SERVO1_PIN 6
#define SERVO2_PIN 5
#define SERVO3_PIN 4
#define MISO_PIN 50
#define MOSI_PIN 51
#define SCK_PIN 52
#define SS_PIN 53
/*void enableSteppers() {
  digitalWrite(X_ENABLE_PIN,LOW);
  digitalWrite(Y_ENABLE_PIN,LOW);
  digitalWrite(Z_ENABLE_PIN,LOW);
  digitalWrite(E0_ENABLE_PIN,LOW);
  digitalWrite(E1_ENABLE_PIN,LOW);
}*/
