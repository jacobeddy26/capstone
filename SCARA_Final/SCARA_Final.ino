/* 
Matthew Costantino
03/24/2024
SCARA_Final.ino

This is the file for the finalized SCARA system code. The code
includes functionality to move the SCARA to a specified position,
pick up a piece at that position, move the SCARA to a second position,
and place the piece down at the second position.

*/

#include <Wire.h>
#include <AccelStepper.h>

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

AccelStepper Inner(1,3,2); //Inner motor with stepPin 3 and dirPin 2
AccelStepper Outer(1,5,4); //Outer motor with stepPin 5 and dirPin 4

#define switch1 10      // Define the electromagnet and servo pins
#define switch2 11
#define actPos 8
#define actNeg 9
#define mag 12    

// Define source and destination squares from received move i.e. "a2b3" = "a2" & "b3"
char src[3], dest[3];

int switchValue1 = 0, switchValue2 = 0;
int holding=0;

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
   {{"a1", 114.0, 21.0}, {"a2", 127.0, 9.0}, {"a3", 158.0, 5.0}, {"a4", 183.0, 11.0}, {"a5", 8.0, 5.0}, {"a6", 8.0, 6.0}, {"a7", 8.0, 7.0}, {"a8", 252.0, 16.0}},
   {{"b1", 122.0, 27.0}, {"b2", 138.0, 15.0}, {"b3", 158.0, 8.0}, {"b4", 183.0, 3.0}, {"b5", 206.0, 3.0}, {"b6", 221.0, 8.0}, {"b7", 227.0, 14.0}, {"b8", 233.0, 24.0}},   
   {{"c1", 127.0, 35.0}, {"c2", 144.0, 24.0}, {"c3", 156.0, 19.0}, {"c4", 174.0, 16.0}, {"c5", 204.0, 19.0}, {"c6", 204.0, 18.0}, {"c7", 213.0, 24.0}, {"c8", 218.0, 33.0}},
   {{"d1", 130.0, 43.0}, {"d2", 143.0, 36.0}, {"d3", 156.0, 30.0}, {"d4", 169.0, 29.0}, {"d5", 194.0, 29.0}, {"d6", 194.0, 30.0}, {"d7", 200.0, 36.0}, {"d8", 205.0, 42.0}},
   {{"e1", 134.0, 55.0}, {"e2", 145.0, 49.0}, {"e3", 157.0, 43.0}, {"e4", 168.0, 41.0}, {"e5", 178.0, 41.0}, {"e6", 188.0, 42.0}, {"e7", 193.0, 48.0}, {"e8", 198.0, 55.0}},
   {{"f1", 126.0, 69.0}, {"f2", 138.0, 62.0}, {"f3", 148.0, 56.0}, {"f4", 158.0, 53.0}, {"f5", 167.0, 53.0}, {"f6", 174.0, 56.0}, {"f7", 178.0, 61.0}, {"f8", 182.0, 69.0}},
   {{"g1", 122.0, 85.0}, {"g2", 132.0, 78.0}, {"g3", 142.0, 74.0}, {"g4", 151.0, 72.0}, {"g5", 157.0, 71.0}, {"g6", 164.0, 74.0}, {"g7", 167.0, 76.0}, {"g8", 170.0, 85.0}},
   {{"h1", 113.0, 108.0}, {"h2", 125.0, 97.0}, {"h3", 132.0, 94.0}, {"h4", 141.0, 89.0}, {"h5", 146.0, 92.0}, {"h6", 152.0, 94.0}, {"h7", 154.0, 101.0}, {"h8", 157.0, 107.0}}
};

Chessboard board(hardcodedAngles);

bool moveReady = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(scaraSA);
  Wire.onReceive(receiveEvent);

  pinMode(switch1,INPUT_PULLUP);
  pinMode(switch2,INPUT_PULLUP);
  pinMode(actPos, OUTPUT);
  pinMode(actNeg,OUTPUT);
  pinMode(mag, OUTPUT);
  digitalWrite(mag,LOW);

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
  //Outer.moveTo(offset);
  //Outer.runToPosition();
  //Inner.moveTo(offset);
  //Inner.runToPosition();
  Inner.setMaxSpeed(90);
  Inner.setAcceleration(30);
  Outer.setMaxSpeed(90);
  Outer.setAcceleration(30);
}

void loop() {
   if (moveReady) {
      makeMove();
   }
}

void pickUpAt(int innerAngle, int outerAngle) {
  
  long innerSteps = -2.88*innerAngle;
  //Serial.println(innerSteps);
  if (innerSteps>0) {
    Inner.move(innerSteps);
  } else {
    Inner.move(innerSteps);
  }
  delay(100);
  long outerSteps = -2.88*outerAngle;
  //Serial.println(outerSteps);
  if (outerSteps>0) {
    Outer.move(outerSteps);
  } else {
    Outer.move(outerSteps);
  }
   delay(100);
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
   Serial.println("Put Down");
  
  int innerSteps = (int)(-2.88*innerAngle);
  if (innerSteps>0) {
    Inner.move(innerSteps);
  }
  else {
    Inner.move(innerSteps);
  }

  int outerSteps = (int)(-2.88*outerAngle);
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

// Function to convert chess notation to source and destination squares
void parseChessMove(char move[6]) {
   src[0] = move[1];  // Source row i.e. a-h
   src[1] = move[2];  // Source column i.e. 1-8
   dest[0] = move[3]; // Destination row
   dest[1] = move[4]; // Destiantion column
}

// Function to receive data over I2C
void receiveEvent() {
   // Define move received from I2C
   char receivedMove[6];
   int i = 0;
   while (Wire.available() && i < 6) {
      digitalWrite(13,HIGH);
      receivedMove[i] = Wire.read(); // Read char data
      i++;
      digitalWrite(13,LOW);
   }
   receivedMove[5] = '\0'; // Null-terminate the received char array
   Serial .print("Received move: ");
   Serial.println(receivedMove); // Print received data to serial monitor
   parseChessMove(receivedMove);
   if (receivedMove[0] == 'o') {
      // Castle
      //Serial.println("Castle");
   } else if (receivedMove[0] == 'x') {
      // Capture
      //Serial.println("Capture");
      // removeCapture()
   } else {
      // Normal
      moveReady=true;
   }
}

void makeMove() {
  char srcX = src[0];
  uint8_t srcY = atoi(&src[1]);
  char destX = dest[0];
  uint8_t destY = atoi(&dest[1]);

  ChessboardSquare &srcSquare = board.getSquare(srcX,srcY);   // Source Square Info
  ChessboardSquare &destSquare = board.getSquare(destX,destY);  // Dest Square Info

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
 
  long innerFirstAngle = 0;
  long outerFirstAngle = 0;
  long innerSecondAngle = 0;
  long outerSecondAngle = 0;

  innerFirstAngle = srcSquare.getTheta1();
  outerFirstAngle = srcSquare.getTheta2();
  innerSecondAngle = destSquare.getTheta1();
  outerSecondAngle = destSquare.getTheta2();

  if (innerFirstAngle != 0) {
    pickUpAt(innerFirstAngle,outerFirstAngle);
  }

  double innerPlaceAngle = innerSecondAngle - innerFirstAngle;
  double outerPlaceAngle = innerSecondAngle - innerFirstAngle;

  if (innerPlaceAngle != 0) {
    putDownAt(innerPlaceAngle,outerPlaceAngle);
  }

  Inner.setMaxSpeed(225);
  Inner.setSpeed(75);
  Outer.setMaxSpeed(225);
  Outer.setSpeed(75);

  if (innerFirstAngle != 0) {  
    switchValue1 = 0;
    switchValue2 = 0;
}

  while (switchValue1 == 0) {
    Inner.runSpeed();
    if (digitalRead(switch1) == HIGH) {
      switchValue1 = 1;
    } else {
      switchValue1 = 0;
    }
  }

  while (switchValue2 == 0) {
    Outer.runSpeed();
    if (digitalRead(switch2) == HIGH) {
      switchValue2 = 1;
    } else {
      switchValue2 = 0;
    }
  }
 
  innerFirstAngle = 0;
  outerFirstAngle = 0;
  innerSecondAngle = 0;
  outerSecondAngle = 0;
  
  moveReady=false;
}
