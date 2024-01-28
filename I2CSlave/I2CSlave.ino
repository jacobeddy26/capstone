#include <Wire.h>

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

#define queen 24
#define knight 8
#define rook 16
#define bishop 12

int promotePiece;
bool flagWhite=true;
bool flagBlack=false;

void setup() {
  Wire.begin(boardSA); // Initialize I2C communication with set slave address
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
  pinMode(3,INPUT);
}

void loop() {
  // Do something with received data
  int val = digitalRead(3);
  if (val != 0) {
   flagTest(flagWhite,flagBlack);
  }
}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
   if(Wire.available()) {
      dataIn = Wire.read();
      selectedPiece(dataIn);
  }
}

void flagTest(bool flagWhite, bool flagBlack) {
    if(flagWhite)
    {
      //send Piece Promotion Flag for White Pieces
      promotePiece = 1000;
    }
    else if(flagBlack) {
      //Send Piece Promotion Flag for Black Pieces
      promotePiece = 1001;
    }
    Serial.println("Promoting!");
    Wire.beginTransmission(engineSA);
    Wire.write(promotePiece);
    Wire.endTransmission();
}

void selectedPiece(int piece) {
  switch (piece)
  {
   case queen:
      Serial.print("Promoted to Queen");         // print the piece
      break;
   case knight:
      Serial.print("Promoted to Knight");         // print the piece
      break;
   case rook:
      Serial.print("Promoted to Rook");         // print the piece
      break;
   case bishop:
      Serial.print("Promoted to Bishop");         // print the piece
      break;
  default:
      Serial.print("ERROR");
      break;
  }
}


