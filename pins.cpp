#include "pins.h"
void definePinMode() {
  pinMode(X_STEP_PIN,OUTPUT);
  pinMode(X_DIR_PIN,OUTPUT);
  pinMode(X_ENABLE_PIN,OUTPUT);
  pinMode(Y_STEP_PIN,OUTPUT);
  pinMode(Y_DIR_PIN,OUTPUT);
  pinMode(Y_ENABLE_PIN,OUTPUT);
  pinMode(Z_STEP_PIN,OUTPUT);
  pinMode(Z_DIR_PIN,OUTPUT);
  pinMode(Z_ENABLE_PIN,OUTPUT);
  pinMode(A_STEP_PIN,OUTPUT);
  pinMode(A_DIR_PIN,OUTPUT);
  pinMode(A_ENABLE_PIN,OUTPUT);
  pinMode(B_STEP_PIN,OUTPUT);
  pinMode(B_DIR_PIN,OUTPUT);
  pinMode(B_ENABLE_PIN,OUTPUT);

  digitalWrite(X_ENABLE_PIN, LOW);
  digitalWrite(Y_ENABLE_PIN, LOW);
  digitalWrite(Z_ENABLE_PIN, LOW);
  digitalWrite(A_ENABLE_PIN, LOW);
  digitalWrite(B_ENABLE_PIN, LOW);

  pinMode(X_MIN_PIN,INPUT);
  pinMode(X_MAX_PIN,INPUT);
  pinMode(Y_MIN_PIN,INPUT);
  pinMode(Y_MAX_PIN,INPUT);
  pinMode(Z_MIN_PIN,INPUT);
  pinMode(Z_MAX_PIN,INPUT);

  noInterrupts();
  TCCR1A = 0;// changing register's time to 0
  TCCR1B = 0;
  TCNT1  = 0;

  OCR1A = 1000;                             // compare value
  TCCR1B |= (1 << WGM12);                   // CTC mode
  TCCR1B |= ((1 << CS11) | (1 << CS10));    // 64 prescaler
  interrupts();
  //procedureInitialize();
}