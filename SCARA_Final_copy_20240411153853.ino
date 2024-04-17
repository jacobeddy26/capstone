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
long innerFirstAngle = 147, outerFirstAngle = 25;
long innerSecondAngle = 160, outerSecondAngle = 72;
long innerPlaceAngle = 0, outerPlaceAngle = 0;
int holding=0;
int castlevar=3;
//long innerCaptureFrom = 208, outerCaptureFrom = 17;
//long innerCaptureAt = 156, outerCaptureAt = 93;


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

  Inner.setMaxSpeed(90);
  Inner.setAcceleration(30);
  Outer.setMaxSpeed(90);
  Outer.setAcceleration(30);

}

void loop() {
/*
  if (innerCaptureAt != 0){
    captureAt(innerCaptureAt,outerCaptureAt,innerCaptureFrom,outerCaptureFrom,outerCaptureAt);
  }


  if (innerFirstAngle != 0) {
    pickUpAt(innerFirstAngle,outerFirstAngle, outerFirstAngle);
  }


  long innerPlaceAngle = innerSecondAngle - innerFirstAngle;
  long outerPlaceAngle = outerSecondAngle - outerFirstAngle;

  if (innerSecondAngle != 0) {
    putDownAt(innerPlaceAngle,outerPlaceAngle, outerSecondAngle);
  }



  if(outerSecondAngle<0) {
    Outer.move(5*outerSecondAngle);
    while(Outer.currentPosition() != Outer.targetPosition()) {
      Outer.run();
    }
  }

  if (outerSecondAngle> 299) {
    Outer.move(50);
    while(Outer.currentPosition() != Outer.targetPosition()) {
      Outer.run();
    }
  }
  */

  castle(castlevar);
  Inner.setMaxSpeed(225);
  Inner.setSpeed(75);
  Outer.setMaxSpeed(225);
  Outer.setSpeed(75);
  
  
if (innerFirstAngle != 0 /*| innerCaptureAt != 0*/) {  
    switchValue1 = 0;
    switchValue2 = 0;
}



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
  castlevar = 0;
  /*
  innerCaptureAt = 0;
  outerCaptureAt = 0;
  innerCaptureFrom = 0;
  outerCaptureFrom = 0;
*/


}

void pickUpAt(int innerAngle, int outerAngle, int absoluteAngle) {
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

  if (absoluteAngle < 0 | absoluteAngle > 300) {
    while(Inner.currentPosition() != Inner.targetPosition()) {
      Inner.run();
    }

    while(Outer.currentPosition() != Outer.targetPosition()) {
        Outer.run();
    }
  }
  else {
    while(Outer.currentPosition() != Outer.targetPosition()) {
      Outer.run();
    }

    while(Inner.currentPosition() != Inner.targetPosition()) {
      Inner.run();
    }
  }

  digitalWrite(actPos, HIGH);
  digitalWrite(actNeg, LOW);
  digitalWrite(mag,HIGH);
  delay(3000);
  digitalWrite(actPos, LOW);
  digitalWrite(actNeg, HIGH);
  delay(2500);
  holding = 1;
}

void putDownAt(int innerAngle, int outerAngle, int absoluteAngle) {
  

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

  if (absoluteAngle < 0 | absoluteAngle > 300) {
    while(Inner.currentPosition() != Inner.targetPosition()) {
      Inner.run();
    }

    while(Outer.currentPosition() != Outer.targetPosition()) {
      Outer.run();
    }
  }
  else {
    while(Outer.currentPosition() != Outer.targetPosition()) {
      Outer.run();
    }

   while(Inner.currentPosition() != Inner.targetPosition()) {
      Inner.run();
    }
  }
  digitalWrite(actPos, HIGH);
  digitalWrite(actNeg, LOW); 
  delay(2500);
  digitalWrite(mag,LOW);
  delay(1000);
  digitalWrite(actPos, LOW);
  digitalWrite(actNeg, HIGH);
  delay(2500);
  holding = 0;
}

void captureAt(int innerCapAt, int outerCapAt, int innerCapFrom, int outerCapFrom, int absoluteAngle) {

  pickUpAt(innerCapAt,outerCapAt,absoluteAngle);

  long innerCapPoint = 137 - innerCapAt;
  long outerCapPoint = 295 - outerCapAt;

  putDownAt(innerCapPoint,outerCapPoint,absoluteAngle);

  long innerCapMove = innerCapFrom - 137;
  long outerCapMove = outerCapFrom - 295;

  pickUpAt(innerCapMove,outerCapMove,absoluteAngle);

  long innerFinalMove = innerCapAt - innerCapFrom;
  long outerFinalMove = outerCapAt - outerCapFrom;

  putDownAt(innerFinalMove, outerFinalMove, absoluteAngle);

}

void castle(int castle) {

  if (castle == 1) {
    pickUpAt(133,55,55);
    putDownAt(-1,-22,33);
    pickUpAt(-13,-15,18);
    putDownAt(16,24,42);
  }
  else if (castle == 2) {
    pickUpAt(133,55,55);
    putDownAt(-8,31,86);
    pickUpAt(-9, 23,109);
    putDownAt(14,-41,68);
  }
  else if (castle == 3) {
    pickUpAt(197,54,54);
    putDownAt(-22,32,86);
    pickUpAt(-14,22,108);
    putDownAt(25,-39,69);
  }
  else if (castle == 4) {
    pickUpAt(197,54,54);
    putDownAt(25,-21,33);
    pickUpAt(30,-16,17);
    putDownAt(-43,25,42);
  }
}





