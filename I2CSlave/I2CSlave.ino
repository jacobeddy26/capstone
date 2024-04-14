#include <Wire.h>

#define scaraSA 3                                // Slave Address for SCARA controller
#define boardSA 2                                // Slave Address for Board/LCD controller
#define engineSA 1                               // Slave Address for Chess Engine controller

char inputMove[5];
char outputMove[6];

void setup(){
   Wire.begin(engineSA);
   Wire.onReceive(receiveEvent);
   Serial.begin(115200);
   Serial.println("  *** CHESSuino ***\n");

   pinMode(13, OUTPUT);
}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
   int i = 0;
   while (Wire.available() && i < 5) {
      inputMove[i] = Wire.read(); // Read char data
      i++;
   }
   inputMove[4] = '\0'; // Null-terminate the received char array
   Serial.print("Input Move: ");
   Serial.println(inputMove); // Print received data to serial monitor
   delay(100);
}