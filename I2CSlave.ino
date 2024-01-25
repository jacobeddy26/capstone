#include <Wire.h>
int engineSA = 1;   // Slave Address for the chess engine controller
int boardSA = 2;    // Slave Address for the board/LCD controller
int scaraSA = 3;    // Slave Address for the SCARA controller

int promotedPiece;
short queen,knight,rook,bishop;
queen = 24;
knight = 8;
rook = 16;
bishop = 12;

void setup() {
  Wire.begin(boardSA); // Initialize I2C communication with set slave address
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  // Do something with received data
}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
  if(Wire.available()) {
    dataIn = Wire.read();
  }
}

void selectedPiece() {
  Wire.requestFrom(engineSA, 2);    // request 2 bytes from engine controller
  while (Wire.available()) { // slave may send less than requested
    char c = Wire.read(); // receive a byte as character
    Serial.print(c);         // print the character
  }
}


