/* 
Matthew Costantino
03/24/2024
SCARA_Final.ino

This is the file for the finalized SCARA system code. The code
includes functionality to move the SCARA to a specified position,
pick up a piece at that position, move the SCARA to a second position,
and place the piece down at the second position.

*/


#include <AccelStepper.h>

AccelStepper Inner(1,3,2); //Inner motor with stepPin 3 and dirPin 2
AccelStepper Outer(1,5,4); //Outer motor with stepPin 5 and dirPin 4

#define switch1 10
#define switch2 11
#define actPos 8
#define actNeg 9
#define mag 12

int switchValue1 = 0, switchValue2 = 0;
long innerFirstAngle = 143, outerFirstAngle = 36;
long innerSecondAngle = 156, outerSecondAngle = 30;
int holding=0;

void setup() {
  pinMode(switch1,INPUT_PULLUP);
  pinMode(switch2,INPUT_PULLUP);
  pinMode(actPos, OUTPUT);
  pinMode(actNeg,OUTPUT);
  pinMode(mag, OUTPUT);
  digitalWrite(mag,LOW);
  Serial.begin(9600);

  Inner.setMaxSpeed(225);
  Inner.setSpeed(75);
  Outer.setMaxSpeed(225);
  Outer.setSpeed(75);

  while (switchValue1 == 0) {
    Inner.runSpeed();
    if (digitalRead(switch1) == HIGH) {
      switchValue1 = 1;
    }
    else {
      switchValue1 = 0;
    }
  }

  while (switchValue2 == 0) {
    Outer.runSpeed();
    if (digitalRead(switch2) == HIGH) {
      switchValue2 = 1;
    }
    else {
      switchValue2 = 0;
    }
  }

  long innerFirstAngle = 100, outerFirstAngle = 20;
  long innerSecondAngle = 140, outerSecondAngle = 40;

  Inner.setMaxSpeed(90);
  Inner.setAcceleration(30);
  Outer.setMaxSpeed(90);
  Outer.setAcceleration(30);

}

void loop() {
  


  pickUpAt(innerFirstAngle,outerFirstAngle);

  long innerPlaceAngle = innerSecondAngle - innerFirstAngle;
  long outerPlaceAngle = innerSecondAngle - innerFirstAngle;
  
  putDownAt(innerPlaceAngle,outerPlaceAngle);

  Inner.setMaxSpeed(225);
  Inner.setSpeed(75);
  Outer.setMaxSpeed(225);
  Outer.setSpeed(75);

  switchValue1 = 0;
  switchValue2 = 0;

  while (switchValue1 == 0) {
    Inner.runSpeed();
    if (digitalRead(switch1) == HIGH) {
      switchValue1 = 1;
    }
    else {
      switchValue1 = 0;
    }
  }

  while (switchValue2 == 0) {
    Outer.runSpeed();
    if (digitalRead(switch2) == HIGH) {
      switchValue2 = 1;
    }
    else {
      switchValue2 = 0;
    }
  }
 
  innerFirstAngle = 0;
  outerFirstAngle = 0;
  innerSecondAngle = 0;
  outerSecondAngle = 0;
}

void pickUpAt(int innerAngle, int outerAngle) {
  int innerSteps = -2.88*innerAngle;
  if (innerSteps>0) {
    Inner.move(innerSteps);
  }
  else {
    Inner.move(innerSteps);
  }

  int outerSteps = -2.88*outerAngle;
  if (outerSteps>0) {
    Outer.move(outerSteps);
  }
  else {
    Outer.move(outerSteps);
  }

  while(Outer.currentPosition() != Outer.targetPosition()) {
    Outer.run();
  }

  while(Inner.currentPosition() != Inner.targetPosition()) {
    Inner.run();
  }

  digitalWrite(actPos, HIGH);
  digitalWrite(actNeg, LOW);
  delay(5000);  
  digitalWrite(mag,HIGH);
  delay(3000);
  digitalWrite(actPos, LOW);
  digitalWrite(actNeg, HIGH);
  delay(5000);
  holding = 1;
}

void putDownAt(int innerAngle, int outerAngle) {
  
  int innerSteps = -2.88*innerAngle;
  if (innerSteps>0) {
    Inner.move(innerSteps);
  }
  else {
    Inner.move(innerSteps);
  }

  int outerSteps = -2.88*outerAngle;
  if (outerSteps>0) {
    Outer.move(outerSteps);
  }
  else {
    Outer.move(outerSteps);
  }

  while(Outer.currentPosition() != Outer.targetPosition()) {
    Outer.run();
  }

  while(Inner.currentPosition() != Inner.targetPosition()) {
    Inner.run();
  }

  digitalWrite(actPos, HIGH);
  digitalWrite(actNeg, LOW);
  delay(5000);  
  digitalWrite(mag,LOW);
  delay(3000);
  digitalWrite(actPos, LOW);
  digitalWrite(actNeg, HIGH);
  delay(5000);
  holding = 0;

}


