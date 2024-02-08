/* 
Matthew Costantino
1/25/2024
Costantino_Stepper_Test.ino

This code is developed in order to control a motor based on 
an internally measured angle that is initially set using a limit
microswitch. The movement angles of the motor will be subdivided
based on the step size if microstepping is used.
*/

//define stepper motor connections:
#define dirPin 2
#define stepPin 3

int currentAngle = 0, nextAngle = 90, moveAngle = 0;
void setup() {
  //Set pin modes for each of the arduino pins
pinMode(stepPin, OUTPUT);
pinMode(dirPin, OUTPUT);
Serial.begin(9600);
}

void loop() {

  //Determine if the angular movement is an increase or decrease in angular position

  //Call a different function depending on angular movement
  if (currentAngle < nextAngle) {
    moveAngle = nextAngle-currentAngle; //Create a move angle variable to send to the function
    moveCW(moveAngle);
    currentAngle = nextAngle; //Set the new current angle
    //Serial.println(currentAngle);
  }
  else if (currentAngle > nextAngle) {
    moveAngle = currentAngle-nextAngle;
    moveCCW(moveAngle);
    currentAngle = nextAngle;
  }
  else {
    currentAngle = nextAngle;
  }
}

/*This function determines the clockwise angular movement required for the motor
and calculates the step movements for the motor to a 0.35 degree accuracy */
void moveCW(int angle) {
  digitalWrite(dirPin,HIGH);
  int steps = (angle/.35);

  for(int i=0; i<steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(5000);
  }
}

/*This function determines the counter clockwise angular movement required for the motor
and calculates the step movements for the motor to a 0.35 degree accuracy */

void moveCCW(int angle) {
digitalWrite(dirPin,LOW);
  int steps = (angle/.35);

  for(int i=0; i<steps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(5000);
  }
}