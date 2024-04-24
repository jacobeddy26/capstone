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
   double innerAngle;
   double outerAngle;
};

// Class to represent a chessboard square
class ChessboardSquare {
private:
   scaraAngle angles;

public:
   // Constructors
   ChessboardSquare() : angles({nullptr, 0.0, 0.0}) {}
   ChessboardSquare(const char *square, double innerAngle, double outerAngle) : angles({square, innerAngle, outerAngle}) {}

   // Getter for theta1
   double getInnerAngle() const {
      return angles.innerAngle;
   }

   // Getter for theta2
   double getOuterAngle() const {
      return angles.outerAngle;
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
            squares[row][col] = ChessboardSquare(hardcodedAngles[row][col]->square, hardcodedAngles[row][col]->innerAngle, hardcodedAngles[row][col]->outerAngle);
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
// Entry: {"square_name", innerAngle, outerAngle}

const scaraAngle hardcodedAngles[8][8][3] = {
   {{"a1", 119.0, 18.0}, {"a2", 134.0, 7.0}, {"a3", 155.0, -3.0}, {"a4", 183.0, -11.0}, {"a5", 45.0, 315.0}, {"a6", 79.0, 309.0}, {"a7", 252.0, 6.0}, {"a8", 252.0, 17.0}},
   {{"b1", 129.0, 24.0}, {"b2", 143.0, 15.0}, {"b3", 162.0, 7.0}, {"b4", 183.0, 3.0}, {"b5", 205.0, 3.0}, {"b6", 220.0, 7.0}, {"b7", 231.0, 15.0}, {"b8", 236.0, 24.0}},   
   {{"c1", 132.0, 33.0}, {"c2", 147.0, 25.0}, {"c3", 162.0, 18.0}, {"c4", 179.0, 15.0}, {"c5", 195.0, 15.0}, {"c6", 207.0, 18.0}, {"c7", 217.0, 24.0}, {"c8", 222.0, 33.0}},
   {{"d1", 135.0, 42.0}, {"d2", 147.0, 35.0}, {"d3", 161.0, 30.0}, {"d4", 173.0, 26.0}, {"d5", 187.0, 26.0}, {"d6", 198.0, 30.0}, {"d7", 204.0, 35.0}, {"d8", 209.0, 42.0}},
   {{"e1", 133.0, 55.0}, {"e2", 146.0, 47.0}, {"e3", 157.0, 43.0}, {"e4", 168.0, 41.0}, {"e5", 178.0, 40.0}, {"e6", 188.0, 42.0}, {"e7", 194.0, 47.0}, {"e8", 197.0, 54.0}},
   {{"f1", 130.0, 68.0}, {"f2", 141.0, 61.0}, {"f3", 151.0, 57.0}, {"f4", 161.0, 55.0}, {"f5", 169.0, 55.0}, {"f6", 177.0, 57.0}, {"f7", 182.0, 61.0}, {"f8", 186.0, 69.0}},
   {{"g1", 125.0, 86.0}, {"g2", 135.0, 79.0}, {"g3", 145.0, 74.0}, {"g4", 153.0, 71.0}, {"g5", 160.0, 72.0}, {"g6", 166.0, 74.0}, {"g7", 171.0, 79.0}, {"g8", 175.0, 86.0}},
   {{"h1", 116.0, 109.0}, {"h2", 126.0, 101.0}, {"h3", 135.0, 94.0}, {"h4", 142.0, 91.0}, {"h5", 149.0, 91.0}, {"h6", 155.0, 93.0}, {"h7", 159.0, 101.0}, {"h8", 161.0, 108.0}}
};

Chessboard board(hardcodedAngles);

bool moveReady = false;
char receivedMove[6];

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

   goHome();
}

void loop() {
   if (moveReady) {
      char srcX = src[0];
      uint8_t srcY = atoi(&src[1]);
      char destX = dest[0];
      uint8_t destY = atoi(&dest[1]);

      ChessboardSquare &srcSquare = board.getSquare(srcX,srcY);   // Source Square Info
      ChessboardSquare &destSquare = board.getSquare(destX,destY);  // Dest Square Info
      
      if (receivedMove[0] == 'o') {
         // Castle
         Serial.println("Castle!");
         castle(srcSquare,destSquare);
      } else if (receivedMove[0] == 'x') {
         Serial.println("Capture!");
         captureAt(srcSquare,destSquare);
      } else {
         // Normal
         Serial.println("Normal");
         pickUpAt(srcSquare);
         putDownAt(srcSquare,destSquare);
      }
      moveReady=false;
      goHome();
   }
}
void pickUpAt(ChessboardSquare &square) {

   int innerAngle=square.getInnerAngle();
   int outerAngle=square.getOuterAngle();
   Serial.print("Picking up at "); Serial.print(square.getName());
   Serial.print(". innerAngle = "); Serial.print(innerAngle); 
   Serial.print(", outerAngle = "); Serial.print(outerAngle);
  
   long innerSteps = -2.88*innerAngle;
   Serial.print(", innerSteps = "); Serial.print(innerSteps);
   Inner.move(innerSteps);
   delay(100);
   long outerSteps = -2.88*outerAngle;
   Serial.print(", outerSteps = "); Serial.print(outerSteps);
   Outer.move(outerSteps);
   delay(100);

   int absoluteAngle = Outer.currentPosition();

   Serial.print(", absoluteAngle = "); Serial.println(absoluteAngle);
   if (absoluteAngle < 0 | absoluteAngle > 300) {
      //Inner.runToPosition();
      while(Inner.currentPosition() != Inner.targetPosition()) {
         Inner.run();
      }
      //Outer.runToPosition();
      while(Outer.currentPosition() != Outer.targetPosition()) {
         Outer.run();
      }
   } else { 
      //Outer.runToPosition();
      while(Outer.currentPosition() != Outer.targetPosition()) {
         Outer.run();
      }
      //Inner.runToPosition();
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

void putDownAt(ChessboardSquare &srcSquare, ChessboardSquare &destSquare) {
   int srcInnerAngle=srcSquare.getInnerAngle();
   int srcOuterAngle=srcSquare.getOuterAngle();
   int destInnerAngle=destSquare.getInnerAngle();
   int destOuterAngle=destSquare.getOuterAngle();
   int innerAngle = destInnerAngle - srcInnerAngle;
   int outerAngle = destOuterAngle - srcOuterAngle;
   Serial.print("Putting down at "); Serial.print(destSquare.getName());
   Serial.print(". innerAngle = "); Serial.print(innerAngle); 
   Serial.print(", outerAngle = "); Serial.print(outerAngle);

   int innerSteps = (int)(-2.88*innerAngle);
   Serial.print(", innerSteps = "); Serial.print(innerSteps);
   Inner.move(innerSteps);

   int outerSteps = (int)(-2.88*outerAngle);
   Serial.print(", outerSteps = "); Serial.print(outerSteps);
   Outer.move(outerSteps);

   int absoluteAngle = Outer.currentPosition();
   Serial.print(", absoluteAngle = "); Serial.println(absoluteAngle);

   if (absoluteAngle < 0 | absoluteAngle > 300) {
      //Inner.runToPosition();
      while(Inner.currentPosition() != Inner.targetPosition()) {
         Inner.run();
      }
      //Outer.runToPosition();
      while(Outer.currentPosition() != Outer.targetPosition()) {
         Outer.run();
      }
   } else {
      //Outer.runToPosition();
      while(Outer.currentPosition() != Outer.targetPosition()) {
         Outer.run();
      }
     // Inner.runToPosition();
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
   int i = 0;
   while (Wire.available() && i < 5) {
      receivedMove[i] = Wire.read(); // Read char data
      i++;
   }
   receivedMove[5] = '\0'; // Null-terminate the received char array
   if ((receivedMove[0] =='?') || (receivedMove[0] == 'x') || receivedMove[0] == 'o')
   {
      Serial .print("Received move: ");
      Serial.println(receivedMove); // Print received data to serial monitor
      parseChessMove(receivedMove);
      moveReady=true;
   }
}

void captureAt(ChessboardSquare &srcSquare, ChessboardSquare &destSquare) {

   int innerCapAt = destSquare.getInnerAngle(); 
   int outerCapAt = destSquare.getOuterAngle();
   int innerCapFrom = srcSquare.getInnerAngle();
   int outerCapFrom = srcSquare.getOuterAngle();

  pickUpAt(destSquare); // Remove piece being captured from square

  long innerCapPoint = 137 - innerCapAt;
  long outerCapPoint = 295 - outerCapAt;

  putDownCapture(innerCapPoint,outerCapPoint); // Place piece in capture zone

  long innerCapMove = innerCapFrom - 137;
  long outerCapMove = outerCapFrom - 295;

  goHome();

  pickUpAt(srcSquare); // Pick piece moving to now empty square

  long innerFinalMove = innerCapAt - innerCapFrom;
  long outerFinalMove = outerCapAt - outerCapFrom;

  putDownAt(srcSquare,destSquare); // Place piece in now empty square

}

void castle(ChessboardSquare &srcSquare, ChessboardSquare &destSquare) {

   ChessboardSquare rookSrc;
   ChessboardSquare rookDest;

   if (destSquare.getName() == "g8") { 
      rookSrc = board.getSquare('h',8);   // Rook Source Square Info
      rookDest = board.getSquare('f',8);  // Rook Dest Square Info
   } else if (destSquare.getName() == "c8") {
      rookSrc = board.getSquare('a',8);   // Rook Source Square Info
      rookDest = board.getSquare('d',8);  // Rook Dest Square Info
   }

   pickUpAt(srcSquare);    // Pick up king
   putDownAt(srcSquare,destSquare);  // Move king to new square
   goHome();
   pickUpAt(rookSrc);      // Pick up rook
   putDownAt(rookSrc,rookDest);    // Move rook to new square
}

void putDownCapture(int innerAngle, int outerAngle) {
  
   int innerSteps = -2.88*innerAngle;
   if (innerSteps>0) {
      Inner.move(innerSteps);
   } else {
      Inner.move(innerSteps);
   }

   int outerSteps = -2.88*outerAngle;
   if (outerSteps>0) {
      Outer.move(outerSteps);
   } else {
      Outer.move(outerSteps);
   }

   int absoluteAngle = Outer.currentPosition();
   if (absoluteAngle < 0 | absoluteAngle > 300) {
      while(Inner.currentPosition() != Inner.targetPosition()) {
         Inner.run();
      }

      while(Outer.currentPosition() != Outer.targetPosition()) {
         Outer.run();
      }
   } else {
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

void goHome() {
   Inner.setMaxSpeed(225);
   Inner.setSpeed(75);
   Outer.setMaxSpeed(225);
   Outer.setSpeed(75);
   switchValue1=0;
   switchValue2=0;

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

   Inner.setMaxSpeed(90);
   Inner.setAcceleration(30);
   Outer.setMaxSpeed(90);
   Outer.setAcceleration(30);
}