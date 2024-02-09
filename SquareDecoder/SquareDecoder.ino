#include <Wire.h>
#include <Servo.h>

// Define the x, y coordinates for each square on the chessboard
const int squareCoordinates[8][8][2] = {
  // Define the x, y coordinates for each square on the chessboard
  // You need to define these coordinates based on your setup
};

// Define the home position for the arm
const int homePosition[2] = { 0,0 }; // { x-coordinate, y-coordinate }

// Define arm dimensions
const float L1 = 7.5; // Length of first arm
const float L2 = 7.5; // Length of second arm

// Define stepper motor control pins
const int stepPin1 = ; // Stepper motor step pin
const int dirPin1 = ; // Stepper motor direction pin
const int stepPin2 = ; // Stepper motor step pin 
const int dirPin2 = ; // Stepper motor direction pin

// Define steps per revolution for the stepper motors
const int stepsPerRevolution = 360 / 0.35;

// Define the electromagnet and servo pins
const int electromagnetPin = 40;
const int servoPin = 38;

// Define the resting and maximum piece height
const int RESTING_HEIGHT = 100;
const int MAX_PIECE_HEIGHT = 56;

// Define the heights of each type of chess piece
const int PIECE_HEIGHTS[] = {41, 34, 20, 28, 24, 19};

// Define the Gripper class
class Gripper {
private:
    Servo servo;
    int previousZ;

public:
    Gripper() : previousZ(-1) {
        pinMode(servoPin, OUTPUT);
        pinMode(electromagnetPin, OUTPUT);
    }

    void calibrate() {
        move(RESTING_HEIGHT);
    }

    void move(int z) {
        z = constrain(z, 0, 100);
        int dc = (z * 0.067) + 4;
        servo.writeMicroseconds(dc * 100);
        int t = (previousZ == -1) ? 1000 : abs(previousZ - z) / 10 + 500;
        delay(t);
        servo.detach();
        previousZ = z;
    }

    void electromagnet(bool on) {
        digitalWrite(electromagnetPin, on ? HIGH : LOW);
    }

    void pickup(int pieceType) {
        int pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(400);
        electromagnet(true);
        delay(200);
        move(RESTING_HEIGHT + pieceHeight);
    }

    void dropoff(int pieceType) {
        int pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(200);
        electromagnet(false);
        delay(400);
        move(RESTING_HEIGHT);
    }

    void cleanup() {
        digitalWrite(electromagnetPin, LOW);
        digitalWrite(servoPin, LOW);
    }
};

Gripper gripper;

// Function to convert chess notation to source and destination squares
void parseChessMove(String move, int &srcX, int &srcY, int &destX, int &destY) {
  srcX = move.charAt(0) - 'a'; // Convert column character to index
  srcY = 8 - (move.charAt(1) - '0'); // Convert row character to index
  destX = move.charAt(2) - 'a'; // Convert column character to index
  destY = 8 - (move.charAt(3) - '0'); // Convert row character to index
}

// Function to calculate inverse kinematics for SCARA arm
void calculateIK(float x, float y, float &theta1, float &theta2) {
  float r = sqrt(x*x + y*y);
  float phi = atan2(y, x);
  
  float A = (L1*L1 - L2*L2 + r*r) / (2 * L1 * r);
  float B = (L1*L1 + L2*L2 - r*r) / (2 * L1 * L2);
  
  theta2 = acos(B);
  theta1 = phi - acos(A);
}

// Function to move the arm to the specified x, y coordinates
void moveArmTo(int x, int y) {
  // Calculate inverse kinematics to get joint angles
  float theta1, theta2;
  calculateIK(x, y, theta1, theta2);
  
  // Convert joint angles to stepper motor steps
  int steps1 = theta1 * stepsPerRevolution / 360;
  int steps2 = theta2 * stepsPerRevolution / 360;
  
  // Move stepper motors to calculated positions
  moveStepper(steps1, stepPin1, dirPin1);
  moveStepper(steps2, stepPin2, dirPin2);
}

// Function to move a stepper motor to the specified number of steps
void moveStepper(int steps, int stepPin, int dirPin) {
  // Set direction
  digitalWrite(dirPin, steps > 0 ? HIGH : LOW);
  
  // Move the motor
  for (int i = 0; i < abs(steps); ++i) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(500); // Adjust this delay for your motor
    digitalWrite(stepPin, LOW);
    delayMicroseconds(500); // Adjust this delay for your motor
  }
}

// Function to receive data over I2C
void receiveEvent(int numBytes) {
  String move = "";
  while (Wire.available()) {
    char c = Wire.read();
    move += c;
  }
  
  int srcX, srcY, destX, destY;
  parseChessMove(move, srcX, srcY, destX, destY);
  
  // Pick up piece from source square
  gripper.pickup(/* pass the piece type */);
  
  // Move arm to destination square
  moveArmTo(squareCoordinates[destX][destY][0], squareCoordinates[destX][destY][1]);
  
  // Put down piece to destination square
  gripper.dropoff(/* pass the piece type */);
  
  // Return arm to home position
  moveArmTo(homePosition[0], homePosition[1]);
}

void setup() {
  // Initialize I2C communication
  Wire.begin(8);                // Join I2C bus as a slave with address 8
  Wire.onReceive(receiveEvent); // Register event
  
  // Set up stepper motor control pins
  pinMode(stepPin1, OUTPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(stepPin2, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  
  // Move arm to home position
  moveArmTo(homePosition[0], homePosition[1]);
  
  // Calibrate the gripper
  gripper.calibrate();
}

void loop() {
  // Nothing to do in the loop as all actions are handled in the receiveEvent function
}
