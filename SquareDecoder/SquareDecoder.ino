#include <Wire.h>
#include <Servo.h>

// Define the home position for the arm
const int homePosition[2] = { 0,0 }; // { x-coordinate, y-coordinate }

// Define stepper motor control pins
const int stepPin1 = 4; // Stepper motor step pin
const int dirPin1 = 5; // Stepper motor direction pin
const int stepPin2 = 6; // Stepper motor step pin 
const int dirPin2 = 7; // Stepper motor direction pin

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

// Define struct scaraAngle
struct scaraAngle {
   String square;
   double theta1;
   double theta2;
};

// Class to represent a chessboard square
class ChessboardSquare {
private:
   scaraAngle angles;

public:
   // Constructors
   ChessboardSquare() : angles({"",0.0, 0.0}) {}
   ChessboardSquare(String square, double theta1, double theta2) : angles({square, theta1, theta2}) {}

   // Getter for theta1
   double getTheta1() const {
      return angles.theta1;
   }

   // Getter for theta2
   double getTheta2() const {
      return angles.theta2;
   }
   
   // Getter for square name
   String getName() const {
      return angles.square;
   }
};

// Class to represent a chessboard with squares
class Chessboard {
private:
   ChessboardSquare squares[8][8];

public:
   // Constructor
   Chessboard(double hardcodedAngles[8][8][2], String squareNames[8][8]) {
      // Initialize angles for each square on the chessboard
      for (int row = 0; row < 8; ++row) {
         for (int col = 0; col < 8; ++col) {
            squares[row][col] = ChessboardSquare(squareNames[row][col], hardcodedAngles[row][col][0], hardcodedAngles[row][col][1]);
         }
      }
   }

   // Getter for accessing a square by its position
   ChessboardSquare& getSquare(char file, int rank) {
      int col = file - 'a';
      int row = 8 - rank;
      return squares[row][col];
   }
};

// Hardcoded angle values for each chessboard position
String squareNames[8][8] { 
   {"h1","h2","h3","h4","h5","h6","h7","h8"},
   {"g1","g2","g3","g4","g5","g6","g7","g8"},
   {"f1","f2","f3","f4","f5","f6","f7","f8"},
   {"e1","e2","e3","e4","e5","e6","e7","e8"},
   {"d1","d2","d3","d4","d5","d6","d7","d8"},
   {"c1","c2","c3","c4","c5","c6","c7","c8"},
   {"b1","b2","b3","b4","b5","b6","b7","b8"},
   {"a1","a2","a3","a4","a5","a6","a7","a8"}
};

double hardcodedAngles[8][8][2] = {
   {{1.0, 1.0}, {1.0, 2.0}, {1.0, 3.0}, {1.0, 4.0}, {1.0, 5.0}, {1.0, 6.0}, {1.0, 7.0}, {1.0, 1.0}},
   {{2.0, 1.0}, {2.0, 2.0}, {2.0, 3.0}, {2.0, 4.0}, {2.0, 5.0}, {2.0, 6.0}, {2.0, 7.0}, {2.0, 8.0}},
   {{3.0, 1.0}, {3.0, 2.0}, {3.0, 3.0}, {3.0, 4.0}, {3.0, 5.0}, {3.0, 6.0}, {3.0, 7.0}, {3.0, 8.0}},
   {{4.0, 1.0}, {4.0, 2.0}, {4.0, 3.0}, {4.0, 4.0}, {4.0, 5.0}, {4.0, 6.0}, {4.0, 7.0}, {4.0, 8.0}},
   {{5.0, 1.0}, {5.0, 2.0}, {5.0, 3.0}, {5.0, 4.0}, {5.0, 5.0}, {5.0, 6.0}, {5.0, 7.0}, {5.0, 8.0}},
   {{6.0, 1.0}, {6.0, 2.0}, {6.0, 3.0}, {6.0, 4.0}, {6.0, 5.0}, {6.0, 6.0}, {6.0, 7.0}, {6.0, 8.0}},
   {{7.0, 1.0}, {7.0, 2.0}, {7.0, 3.0}, {7.0, 4.0}, {7.0, 5.0}, {7.0, 6.0}, {7.0, 7.0}, {7.0, 8.0}},
   {{1.0, 1.0}, {8.0, 2.0}, {8.0, 3.0}, {8.0, 4.0}, {8.0, 5.0}, {8.0, 6.0}, {8.0, 7.0}, {8.0, 8.0}}
};

Chessboard board(hardcodedAngles,squareNames);

ChessboardSquare& a1 = board.getSquare('a', 1); ChessboardSquare& a2 = board.getSquare('a', 2);
ChessboardSquare& a3 = board.getSquare('a', 3); ChessboardSquare& a4 = board.getSquare('a', 4);
ChessboardSquare& a5 = board.getSquare('a', 5); ChessboardSquare& a6 = board.getSquare('a', 6);
ChessboardSquare& a7 = board.getSquare('a', 7); ChessboardSquare& a8 = board.getSquare('a', 8);
ChessboardSquare& b1 = board.getSquare('b', 1); ChessboardSquare& b2 = board.getSquare('b', 2);
ChessboardSquare& b3 = board.getSquare('b', 3); ChessboardSquare& b4 = board.getSquare('b', 4);
ChessboardSquare& b5 = board.getSquare('b', 5); ChessboardSquare& b6 = board.getSquare('b', 6);
ChessboardSquare& b7 = board.getSquare('b', 7); ChessboardSquare& b8 = board.getSquare('b', 8);
ChessboardSquare& c1 = board.getSquare('c', 1); ChessboardSquare& c2 = board.getSquare('c', 2);
ChessboardSquare& c3 = board.getSquare('c', 3); ChessboardSquare& c4 = board.getSquare('c', 4);
ChessboardSquare& c5 = board.getSquare('c', 5); ChessboardSquare& c6 = board.getSquare('c', 6);
ChessboardSquare& c7 = board.getSquare('c', 7); ChessboardSquare& c8 = board.getSquare('c', 8);
ChessboardSquare& d1 = board.getSquare('d', 1); ChessboardSquare& d2 = board.getSquare('d', 2);
ChessboardSquare& d3 = board.getSquare('d', 3); ChessboardSquare& d4 = board.getSquare('d', 4);
ChessboardSquare& d5 = board.getSquare('d', 5); ChessboardSquare& d6 = board.getSquare('d', 6);
ChessboardSquare& d7 = board.getSquare('d', 7); ChessboardSquare& d8 = board.getSquare('d', 8);
ChessboardSquare& e1 = board.getSquare('e', 1); ChessboardSquare& e2 = board.getSquare('e', 2);
ChessboardSquare& e3 = board.getSquare('e', 3); ChessboardSquare& e4 = board.getSquare('e', 4);
ChessboardSquare& e5 = board.getSquare('e', 5); ChessboardSquare& e6 = board.getSquare('e', 6);
ChessboardSquare& e7 = board.getSquare('e', 7); ChessboardSquare& e8 = board.getSquare('e', 8);
ChessboardSquare& f1 = board.getSquare('f', 1); ChessboardSquare& f2 = board.getSquare('f', 2);
ChessboardSquare& f3 = board.getSquare('f', 3); ChessboardSquare& f4 = board.getSquare('f', 4);
ChessboardSquare& f5 = board.getSquare('f', 5); ChessboardSquare& f6 = board.getSquare('f', 6);
ChessboardSquare& f7 = board.getSquare('f', 7); ChessboardSquare& f8 = board.getSquare('f', 8);
ChessboardSquare& g1 = board.getSquare('g', 1); ChessboardSquare& g2 = board.getSquare('g', 2);
ChessboardSquare& g3 = board.getSquare('g', 3); ChessboardSquare& g4 = board.getSquare('g', 4);
ChessboardSquare& g5 = board.getSquare('g', 5); ChessboardSquare& g6 = board.getSquare('g', 6);
ChessboardSquare& g7 = board.getSquare('g', 7); ChessboardSquare& g8 = board.getSquare('g', 8);
ChessboardSquare& h1 = board.getSquare('h', 1); ChessboardSquare& h2 = board.getSquare('h', 2);
ChessboardSquare& h3 = board.getSquare('h', 3); ChessboardSquare& h4 = board.getSquare('h', 4);
ChessboardSquare& h5 = board.getSquare('h', 5); ChessboardSquare& h6 = board.getSquare('h', 6);
ChessboardSquare& h7 = board.getSquare('h', 7); ChessboardSquare& h8 = board.getSquare('h', 8);  

void setup() {
   Serial.begin(115200);

}

void loop() {
   
   Serial.print("Square ");
   Serial.print(a1.getName());
   Serial.print(": theta1 = ");
   Serial.print(a1.getTheta1());
   Serial.print(", theta2 = ");
   Serial.println(a1.getTheta2()); 
   
   delay(2000);
}

// Function to convert chess notation to source and destination squares
void parseChessMove(char move[6], char &srcX, char &srcY, char &destX, char &destY) {
  srcX = move[1]; // Convert column character to index
  srcY = move[2]; // Convert row character to index
  destX = move[3]; // Convert column character to index
  destY = move[4]; // Convert row character to index
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
      //removeCapture()
   } else {
      // Normal
      //parseChessMove(receivedMove);
      //makeMove();
   }
}
