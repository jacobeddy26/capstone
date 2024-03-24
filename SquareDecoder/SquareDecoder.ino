#include <Wire.h>
#include <Servo.h>

// Define the x, y coordinates for each square on the chessboard
const int squareCoordinates[8][8][2] = {
  // Define the x, y coordinates for each square on the chessboard
  // You need to define these coordinates based on your setup
};

// Define the home position for the arm
const int homePosition[2] = { 0,0 }; // { x-coordinate, y-coordinate }

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

// Define move received from I2C
char receivedMove[6];

// Define individual move components (row, column) i.e. "a2b3" = "a2" & "b3"
char srcX, srcY, destX, destY;

struct SCARAArmAngles {
    double theta1; // Angle for the first joint
    double theta2; // Angle for the second joint
};

SCARAArmAngles chessboard[64]= {}

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
void parseChessMove(char move[6], char &srcX, char &srcY, char &destX, char &destY) {
  srcX = move[1]; // Convert column character to index
  srcY = move[2]; // Convert row character to index
  destX = move[3]; // Convert column character to index
  destY = move[4]; // Convert row character to index
}

// Function to move the arm to the specified x, y coordinates
void moveArmTo(int x, int y) {
  // Calculate inverse kinematics to get joint angles
  float theta1, theta2;
  
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

// Function to receive data over I2C
void receiveEvent() {
   int i = 0;
   while (Wire.available() && i < 6) {
      receivedMove[i] = Wire.read(); // Read char data
      i++;
   }
   receivedMove[5] = '\0'; // Null-terminate the received char array
   Serial.println(receivedMove); // Print received data to serial monitor

   if (receivedMove[0]=='o') {
      // Castle
   } else if(receivedMove[0]=='x') {
      // Capture
      removeCapture()
   } else {
      // Normal
      parseChessMove(receivedMove);
      makeMove();
   }
}
