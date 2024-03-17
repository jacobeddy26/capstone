/*
Matthew Costantino
ECE-4910-01
03/16/2024
calibration_test.ino
This code will be used to calibrate each of the 64 various board positions. 
On startup of the file initially, both motors will spin in their respective
directions until they reach the limit switches. After this, the code will 
request user input to move to the angle and following this it will
request attional motor changes. The user will use this code along with the 
board itself to calibrate the motors to each board position.
*/

#define dirPin1 2
#define stepPin1 3
#define dirPin2 4
#define stepPin2 5
#define switch1 10
#define switch2 11


int currentAngle1 = 0;
int currentAngle2 = 0;
int switchValue1 = 0, switchValue2 = 0;

void setup() {
  //Set pin modes for each of the arduino pins
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(switch1,INPUT_PULLUP);
  pinMode(switch2,INPUT_PULLUP);
  Serial.begin(9600);

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
    digitalWrite(stepPin1, HIGH);
    delayMicroseconds(500);
    digitalWrite(stepPin2, LOW);
    delayMicroseconds(5000);
    if (digitalRead(switch2) == HIGH) {
      switchValue2 = 1;
    }
    else {
      switchValue2 = 0;
    }
    delay(2000);
  }
}


void loop() {

  Serial.println("Please enter angular movement: ");
  while(Serial.available() == 0) {
  }
  float var1=Serial.parseFloat();
  while(Serial.available() == 0) {
  }
  float var2=Serial.parseFloat();

  Serial.println(var1);
  Serial.println(var2);

  if (var1 > 0) {
    moveCW(var1, 1);
    currentAngle1 = currentAngle1 + var1;
  }
  else if (var1 < 0) {
    moveCCW(-var1,1);
    currentAngle1 = currentAngle1 + var1;
  }
  else {
    currentAngle1 = currentAngle1;
  }

  delay(1000);

  if (var2 > 0) {
    moveCW(var2, 2);
    currentAngle2 = currentAngle2 + var2;
  }
  else if (var2 < 0) {
    moveCCW(-var2, 2);
    currentAngle2 = currentAngle2 + var2;
  }
  else {
    currentAngle2 = currentAngle2;
  }

  delay(1000);

  Serial.println("New Current Angles: ");
  Serial.println(currentAngle1);
  Serial.println(currentAngle2);


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
