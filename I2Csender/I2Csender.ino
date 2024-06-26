#include <Wire.h>

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

// standard Arduino setup()
void setup()
{
   Serial.begin(115200);
   Wire.begin(boardSA); // Initialize I2C communication with set slave address
   Wire.onReceive(receiveEvent);
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

   char userMove[5];
   //char userMove[6];
   for(int i = 0; i < 4; i++) {
      userMove[i] = inputString.charAt(i);
   }
   userMove[4]='\0';
   //userMove[5]='\0';
   if(inputString.compareTo("none")!=0) {
      Serial.print("Sending move: ");
      Serial.println(userMove);
      Wire.beginTransmission(engineSA);
      //Wire.beginTransmission(scaraSA);
      Wire.write(userMove,5);
      //Wire.write(userMove,6);
      Wire.endTransmission();
   }
}

// Function to receive data over I2C
void receiveEvent() {
   // Define move received from I2C
   char receivedMove[6];
   int i = 0;
   while (Wire.available() && i < 6) {
      receivedMove[i] = Wire.read(); // Read char data
      i++;
   }
   receivedMove[5] = '\0'; // Null-terminate the received char array
   delay(100);
   if ((receivedMove[0] =='?') || (receivedMove[0] == 'x') || receivedMove[0] == 'o')
   {
      Serial .print("Received move: ");
      Serial.println(receivedMove); // Print received data to serial monitor
   }
}