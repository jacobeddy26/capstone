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
#define dirPin1 2
#define stepPin1 3
#define dirPin2 4
#define stepPin2 5
#define switch1 10
#define switch2 11

int currentAngle1 = 0, nextAngle1 = 90, moveAngle1 = 0;
int currentAngle2 = 0, nextAngle2 = 90, moveAngle2 = 0;
int switchValue1 = 0, switchValue2 = 0;

void setup() {
  //Set pin modes for each of the arduino pins
pinMode(stepPin1, OUTPUT);
pinMode(dirPin1, OUTPUT);
pinMode(stepPin2, OUTPUT);
pinMode(dirPin2, OUTPUT);
pinMode(switch1,INPUT_PULLUP);
Serial.begin(9600);
}
void loop() {
    
while (switchValue1 == 0) {
  digitalWrite(dirPin1, HIGH);
  digitalWrite(stepPin1, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin1, LOW);
  delayMicroseconds(5000);
  if (digitalRead(switch1) == HIGH) {
    switchValue1 = 1;
  }
  else {
    switchValue1 = 0;
  }
}

while (switchValue2 == 0) {
  digitalWrite(dirPin2, HIGH);
  digitalWrite(stepPin1=2, HIGH);
  delayMicroseconds(500);
  digitalWrite(stepPin2, LOW);
  delayMicroseconds(5000);
  if (digitalRead(switch2) == HIGH) {
    switchValue2 = 1;
  }
  else {
    switchValue2 = 0;
  }
}


delay(2000);
  //Determine if the angular movement is an increase or decrease in angular position

  //Call a different function depending on angular movement
  if (currentAngle1 < nextAngle1) {
    moveAngle1 = nextAngle1-currentAngle1; //Create a move angle variable to send to the function
    moveCW(moveAngle1,1);
    currentAngle1 = nextAngle1; //Set the new current angle
  }
  else if (currentAngle1 > nextAngle1) {
    moveAngle1 = currentAngle1-nextAngle1;
    moveCCW(moveAngle1,1);
    currentAngle1 = nextAngle1;
  }
  else {
    currentAngle1 = nextAngle1;
  }

  if (currentAngle2 < nextAngle2) {
    moveAngle2 = nextAngle2-currentAngle2; //Create a move angle variable to send to the function
    moveCW(moveAngle2,2);
    currentAngle2 = nextAngle2; //Set the new current angle
  }
  else if (currentAngle2 > nextAngle2) {
    moveAngle2 = currentAngle2-nextAngle2;
    moveCCW(moveAngle2,2);
    currentAngle2 = nextAngle2;
  }
  else {
    currentAngle2 = nextAngle2;
  }
}

/*This function determines the clockwise angular movement required for the motor
and calculates the step movements for the motor to a 0.35 degree accuracy */
void moveCW(int angle, int motor) {
  int steps = (angle/.35);

  if (motor == 1) {
    digitalWrite(dirPin1,HIGH);
    for(int i=0; i<steps; i++) {
      digitalWrite(stepPin1, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin1, LOW);
      delayMicroseconds(5000);
    }
  }
  else if (motor == 2) {
    digitalWrite(dirPin2,HIGH);
    for(int i=0; i<steps; i++) {
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(5000);
    }
  }

  
}

/*This function determines the counter clockwise angular movement required for the motor
and calculates the step movements for the motor to a 0.35 degree accuracy */

void moveCCW(int angle, int motor) {
int steps = (angle/.35);

  if (motor == 1) {
    digitalWrite(dirPin1,LOW);
    for(int i=0; i<steps; i++) {
      digitalWrite(stepPin1, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin1, LOW);
      delayMicroseconds(5000);
    }
  }
  else if (motor == 2) {
    digitalWrite(dirPin2,LOW);
    for(int i=0; i<steps; i++) {
      digitalWrite(stepPin2, HIGH);
      delayMicroseconds(500);
      digitalWrite(stepPin2, LOW);
      delayMicroseconds(5000);
    }
  }

}