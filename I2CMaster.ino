// Code for Engine Controller & LCD I2C Promotion Test

#include <Wire.h>
int engineSA = 1;   // Slave Address for the chess engine controller
int boardSA = 2;    // Slave Address for the board/LCD controller
int scaraSA = 3;    // Slave Address for the SCARA controller

int dataIn, whitePromotion, backPromotion;
whitePromotion = 1000;
blackPromotion = 1001;

int promotedPiece;
short queen,knight,rook,bishop;
queen = 24;
knight = 8;
rook = 16;
bishop = 12;

void setup() {
  Wire.begin(engineSA); // Initialize I2C communication as master
  Wire.onReceive(receiveEvent);
  Wire.onRequest(sendPromotion);
  Serial.begin(9600);
}

void loop() {
  /*
  char dataToSend[] = "b2b40"; // Char array of length 5
  Wire.beginTransmission(scaraSA); // Slave address (change as needed)
  Wire.write(dataToSend, 5); // Send the char array
  Wire.endTransmission();
  delay(1000); // Wait for a second before sending again
  */

  switch(dataIn) {
    case whitePromotion: 
      // Prompt player (white) for piece to promote to
      promotedPiece = 0;
      break;
    case blackPromotion:
      // Prompt player (black) for piece to promote to
      promotedPiece = 0;
      break;
  }

}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
  if(Wire.available()) {
    dataIn = Wire.read()
  }
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  Wire.write(promotedPiece); // respond with message of 6 bytes
  // as expected by master
}
