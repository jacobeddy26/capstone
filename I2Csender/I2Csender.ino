#include "Wire.h"

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

// standard Arduino setup()
void setup()
{
    Serial.begin(115200);
    Wire.begin(boardSA); // Initialize I2C communication with set slave address
}

void loop() 
{
   String inputString = "none";
   if (Serial.available() > 0) {
    // Read the string from the serial port
    inputString = Serial.readString();

    // Print the received string
    Serial.print("Received String: ");
    Serial.println(inputString);
  }

  char userMove[6];
   for(int i = 0; i < 5; i++) {
      userMove[i] = inputString.charAt(i);
   }
   userMove[5]='\0';
   if(inputString.compareTo("none")!=0) {
      Serial.print("Sending move: ");
      Serial.println(userMove);
      Wire.beginTransmission(scaraSA);
      Wire.write(userMove,6);
      Wire.endTransmission();
   }
}