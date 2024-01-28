#include <Wire.h>
int engineSA = 1;   // Slave Address for the chess engine controller
int boardSA = 2;    // Slave Address for the board/LCD controller
int scaraSA = 3;    // Slave Address for the SCARA controller

char receivedMove[5];

void setup() {
  Wire.begin(scaraSA); // Initialize I2C communication as slave with address 8
  Wire.onReceive(receiveEvent);
  Serial.begin(9600);
}

void loop() {
  // Do something with received data
}

void receiveEvent() {
  int i = 0;
  while (Wire.available() && i < 5) {
    receivedMove[i] = Wire.read(); // Read char data
    i++;
  }
  receivedMove[4] = '\0'; // Null-terminate the received char array
  Serial.println(receivedMove); // Print received data to serial monitor
}
