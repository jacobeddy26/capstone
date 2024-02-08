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

int currentAngle1 = 0, moveAngle1 = 0;
int currentAngle2 = 0, moveAngle2 = 0;

double theta1, theta2;

void setup() {
   //Set pin modes for each of the arduino pins
   pinMode(stepPin, OUTPUT);
   pinMode(dirPin, OUTPUT);
   Serial.begin(9600);
}

void loop() {

}

void moveArmToSquare(double x,  double y) {
   // Inverse Kinematics Calculation
   calculateAngles(x, y);

   // Move the SCARA arm to the calculated joint angles
   moveShoulder();
   moveElbow();
   // Optionally, moveWrist and controlGripper functions can be added if needed
   
   // Delay to allow the arm to reach the desired position
   delay(1000);
}

void calculateAngles(double x, double y) {		// Find Theta1 and Theta2
   // Inverse Kinematics Calculation
   double L1 = 7.5;
   double L2 = 7.5;

   double D = (pow(x,2) + pow(y,2) - pow(L1,2) - pow(L2,2))/ (2 * L1 * L2);
   theta2 = atan2(-sqrt(1 - pow(D,2)), D);
    
   double phi = atan2(y, x); 			// Ensure correct quadrant
   double c = L1 + L2 * cos(theta2);
   double s = L2 * sin(theta2);
   theta1 = phi - atan2(s, c);
    
   // Update global variables or pass theta1 and theta2 to the calling function
}

void moveShoulder() {
   //Determine if the angular movement is an increase or decrease in angular position

  //Call a different function depending on angular movement
  if (currentAngle1 < theta1) {
    moveAngle1 = theta1-currentAngle1; //Create a move angle variable to send to the function
    moveCW(moveAngle1);
    currentAngle1 = theta1; //Set the new current angle
    //Serial.println(currentAngle);
  }
  else if (currentAngle2 > theta2) {
    moveAngle2 = currentAngle2-theta2;
    moveCCW(moveAngle2);
    currentAngle2 = theta2;
  }
  else {
    currentAngle2 = theta2;
  }

}

void moveElbow() {
   
  //Call a different function depending on angular movement
  if (currentAngle1 < theta1) {
    moveAngle1 = theta1-currentAngle1; //Create a move angle variable to send to the function
    moveCW(moveAngle1);
    currentAngle1 = theta1; //Set the new current angle
    //Serial.println(currentAngle);
  }
  else if (currentAngle2 > theta2) {
    moveAngle2 = currentAngle2-theta2;
    moveCCW(moveAngle2);
    currentAngle2 = theta2;
  }
  else {
    currentAngle2 = theta2;
  }

}

// Example Usage:
// moveArmToSquare(4, 3)  // Move to square (4, 3) on the chessboard

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