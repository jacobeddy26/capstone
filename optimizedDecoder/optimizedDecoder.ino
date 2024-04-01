#include <Wire.h>
#include <Servo.h>

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

// Define the home position for the arm
const int homePosition[2] = {0, 0}; // { x-coordinate, y-coordinate }

// Define stepper motor control pins
const uint8_t stepPin1 = 4; // Stepper motor step pin
const uint8_t dirPin1 = 5;  // Stepper motor direction pin
const uint8_t stepPin2 = 6; // Stepper motor step pin
const uint8_t dirPin2 = 7;  // Stepper motor direction pin

// Define steps per revolution for the stepper motors
const uint16_t stepsPerRevolution = 360 / 0.35;

// Define the electromagnet and servo pins
const uint8_t electromagnetPin = 40;
const uint8_t servoPin = 38;

// Define the resting and maximum piece height
const uint8_t RESTING_HEIGHT = 100;
const uint8_t MAX_PIECE_HEIGHT = 56;

// Define the heights of each type of chess piece
const uint8_t PIECE_HEIGHTS[] = {41, 34, 20, 28, 24, 19};

// Define move received from I2C
char receivedMove[6];

// Define source and destination squares from received move i.e. "a2b3" = "a2" & "b3"
char src[3], dest[3];

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

    void move(uint8_t z) {
        z = constrain(z, 0, 100);
        uint8_t dc = (z * 0.067) + 4;
        servo.writeMicroseconds(dc * 100);
        int t = (previousZ == -1) ? 1000 : abs(previousZ - z) / 10 + 500;
        delay(t);
        servo.detach();
        previousZ = z;
    }

    void electromagnet(bool on) {
        digitalWrite(electromagnetPin, on ? HIGH : LOW);
    }

    void pickup(uint8_t pieceType) {
        uint8_t pieceHeight = PIECE_HEIGHTS[pieceType];
        move(pieceHeight);
        delay(400);
        electromagnet(true);
        delay(200);
        move(RESTING_HEIGHT + pieceHeight);
    }

    void dropoff(uint8_t pieceType) {
        uint8_t pieceHeight = PIECE_HEIGHTS[pieceType];
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

// Define struct scaraAngle
struct scaraAngle {
   const char *square;
   double theta1;
   double theta2;
};

// Class to represent a chessboard square
class ChessboardSquare {
private:
   scaraAngle angles;

public:
   // Constructors
   ChessboardSquare() : angles({nullptr, 0.0, 0.0}) {}
   ChessboardSquare(const char *square, double theta1, double theta2) : angles({square, theta1, theta2}) {}

   // Getter for theta1
   double getTheta1() const {
      return angles.theta1;
   }

   // Getter for theta2
   double getTheta2() const {
      return angles.theta2;
   }

   // Getter for square name
   const char *getName() const {
      return angles.square;
   }
};

// Class to represent a chessboard with squares
class Chessboard {
private:
   ChessboardSquare squares[8][8];

public:
   // Constructor
   Chessboard(const scaraAngle hardcodedAngles[8][8][3]) {
      // Initialize angles for each square on the chessboard
      for (uint8_t row = 0; row < 8; ++row) {
         for (uint8_t col = 0; col < 8; ++col) {
            squares[row][col] = ChessboardSquare(hardcodedAngles[row][col]->square, hardcodedAngles[row][col]->theta1, hardcodedAngles[row][col]->theta2);
         }
      }
   }

   // Getter for accessing a square by its position
   ChessboardSquare &getSquare(char file, uint8_t rank) {
      uint8_t row = file - 'a';
      uint8_t col = rank - 1;
      return squares[row][col];
   }
};

// Hardcoded names and angle values for each chessboard position

const scaraAngle hardcodedAngles[8][8][3] = {
   {{"a1", 1.0, 1.0}, {"a2", 8.0, 2.0}, {"a3", 8.0, 3.0}, {"a4", 8.0, 4.0}, {"a5", 8.0, 5.0}, {"a6", 8.0, 6.0}, {"a7", 8.0, 7.0}, {"a8", 8.0, 8.0}},
   {{"b1", 7.0, 1.0}, {"b2", 7.0, 2.0}, {"b3", 7.0, 3.0}, {"b4", 7.0, 4.0}, {"b5", 7.0, 5.0}, {"b6", 7.0, 6.0}, {"b7", 7.0, 7.0}, {"b8", 7.0, 8.0}},   
   {{"c1", 6.0, 1.0}, {"c2", 6.0, 2.0}, {"c3", 6.0, 3.0}, {"c4", 6.0, 4.0}, {"c5", 6.0, 5.0}, {"c6", 6.0, 6.0}, {"c7", 6.0, 7.0}, {"c8", 6.0, 8.0}},
   {{"d1", 5.0, 1.0}, {"d2", 5.0, 2.0}, {"d3", 5.0, 3.0}, {"d4", 5.0, 4.0}, {"d5", 5.0, 5.0}, {"d6", 5.0, 6.0}, {"d7", 5.0, 7.0}, {"d8", 5.0, 8.0}},
   {{"e1", 4.0, 1.0}, {"e2", 4.0, 2.0}, {"e3", 4.0, 3.0}, {"e4", 4.0, 4.0}, {"e5", 4.0, 5.0}, {"e6", 4.0, 6.0}, {"e7", 4.0, 7.0}, {"e8", 4.0, 8.0}},
   {{"f1", 3.0, 1.0}, {"f2", 3.0, 2.0}, {"f3", 3.0, 3.0}, {"f4", 3.0, 4.0}, {"f5", 3.0, 5.0}, {"f6", 3.0, 6.0}, {"f7", 3.0, 7.0}, {"f8", 3.0, 8.0}},
   {{"g1", 2.0, 1.0}, {"g2", 2.0, 2.0}, {"g3", 2.0, 3.0}, {"g4", 2.0, 4.0}, {"g5", 2.0, 5.0}, {"g6", 2.0, 6.0}, {"g7", 2.0, 7.0}, {"g8", 2.0, 8.0}},
   {{"h1", 1.0, 1.0}, {"h2", 1.0, 2.0}, {"h3", 1.0, 3.0}, {"h4", 1.0, 4.0}, {"h5", 1.0, 5.0}, {"h6", 1.0, 6.0}, {"h7", 1.0, 7.0}, {"h8", 1.0, 1.0}}
};

Chessboard board(hardcodedAngles);

void setup() {
   Serial.begin(115200);
   Wire.begin(scaraSA);
   Wire.onReceive(receiveEvent);
}

void loop() {

}

// Function to convert chess notation to source and destination squares
void parseChessMove(char move[6]) {
   src[0] = move[1];  // Source row i.e. a-h
   src[1] = move[2];  // Source column i.e. 1-8
   dest[0] = move[3]; // Destination row
   dest[1] = move[4]; // Destiantion column
   /*
   Serial.print(src[0]);
   Serial.print(src[1]);
   Serial.print(dest[0]);
   Serial.println(dest[1]);
   */
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

   if (receivedMove[0] == 'o') {
      // Castle
      Serial.println("Castle");
   } else if (receivedMove[0] == 'x') {
      // Capture
      Serial.println("Capture");
      // parseChessMove()
      // removeCapture()
   } else {
      // Normal
      parseChessMove(receivedMove);
      makeMove();
   }
}

void makeMove() {
   char srcX = src[0];
   uint8_t srcY = atoi(&src[1]);
   char destX = dest[0];
   uint8_t destY = atoi(&dest[1]);
   
   Serial.print(srcX);
   Serial.print(srcY);
   Serial.print(destX);
   Serial.println(destY);

   ChessboardSquare &srcSquare = board.getSquare(srcX,srcY);
   ChessboardSquare &destSquare = board.getSquare(destX,destY);
   Serial.print("Source Square ");
   Serial.print(srcSquare.getName());
   Serial.print(": theta1 = ");
   Serial.print(srcSquare.getTheta1());
   Serial.print(", theta2 = ");
   Serial.println(srcSquare.getTheta2());
   Serial.print("Destination Square ");
   Serial.print(destSquare.getName());
   Serial.print(": theta1 = ");
   Serial.print(destSquare.getTheta1());
   Serial.print(", theta2 = ");
   Serial.println(destSquare.getTheta2());
}

