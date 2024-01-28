/*************************************************** 
  This is a library for the CAP1188 I2C/SPI 8-chan Capacitive Sensor

  Designed specifically to work with the CAP1188 sensor from Adafruit
  ----> https://www.adafruit.com/products/1602

  These sensors use I2C/SPI to communicate, 2+ pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/
 
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>

// Reset Pin is used for I2C or SPI
#define CAP1188_RESET  9

// CS pin is used for software or hardware SPI
#define CAP1188_CS  10

// These are defined for software SPI, for hardware SPI, check your 
// board's SPI pins in the Arduino documentation
#define CAP1188_MOSI  11
#define CAP1188_MISO  12
#define CAP1188_CLK  13

// For I2C, connect SDA to your Arduino's SDA pin, SCL to SCL pin
// On UNO/Duemilanove/etc, SDA == Analog 4, SCL == Analog 5
// On Leonardo/Micro, SDA == Digital 2, SCL == Digital 3
// On Mega/ADK/Due, SDA == Digital 20, SCL == Digital 21

// Use I2C, no reset pin!
Adafruit_CAP1188 cap = Adafruit_CAP1188();

// Or...Use I2C, with reset pin
//Adafruit_CAP1188 cap = Adafruit_CAP1188(CAP1188_RESET);

//This array contains the binary version of the current board that the inputs from the capacitive touch sensors will be compared against to see if a move has been made
int old_boardState[64]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

// piece type and location
//0=none, 1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king
int typesAndLocations[64]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

//Counter recording how many moves have been made
int NumberOfMovesMade=1;

//Array containing color of each piece
//0=no piece, 1=white, 2=black
int PieceColors[64]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0};

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

int promotedPiece;
#define queen 24
#define knight 8
#define rook 16
#define bishop 12

bool flagWhite=true;
bool flagBlack=false;

void setup() {
  Serial.begin(9600);
  Wire.begin(boardSA); // Initialize I2C communication with set slave address
  Wire.onReceive(receiveEvent);
  /*
  Serial.println("CAP1188 test!");

  // Initialize the sensor, if using i2c you can pass in the i2c address
  // if (!cap.begin(0x28)) {
  if (!cap.begin()) {
    Serial.println("CAP1188 not found");
    while (1);
  }
  Serial.println("CAP1188 found!");
  */
}

void loop() {

  delay(3000);
  uint8_t touched = cap.touched();
  int input[64];
  int PieceWasHere=-1;
  int AnotherPieceWasHere=-1; //Variable used when capturing pieces
  int PieceMovedHere=-1;
  int NumberOfChanges=0;
  flagTest(flagWhite,flagBlack);
}

void selectedPiece(int piece) {
  Wire.requestFrom(engineSA, 2);    // request 2 bytes from engine controller
  while (Wire.available()) { // slave may send less than requested
    piece = Wire.read(); // receive a byte as character
  }
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

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
  int dataIn;
  if(Wire.available()) {
    dataIn = Wire.read();
  }
  selectedPiece(dataIn);

}

void flagTest(bool flagWhite, bool flagBlack) {
    if(flagWhite)
    {
      //send Piece Promotion Flag for White Pieces
      promotedPiece = 1000;
    }
    else if(flagBlack) {
      //Send Piece Promotion Flag for Black Pieces
      promotedPiece = 1001;
    }
    Serial.println("Promoting!");
    Wire.beginTransmission(engineSA);
    Wire.write(promotedPiece);
    Wire.endTransmission();
}
/*
//This block creates an input array from the sensor inputs, and compares the input array to the current boardstate known by the computer. 
//By comparing these arrays, the computer can identify moves made by the user when there are differences between the arrays.
  for (uint8_t i=0; i<8; i++) 
  {
    if (touched & (1 << i)) 
    {
    //Serial.print("C"); Serial.print(i+1); Serial.print("\t");
      input[i]=1;
    }
    else
    {
      input[i]=0;
    }

    if(input[i]!=old_boardState[i])
    {
      if(old_boardState[i]==1)
      {
        if(PieceWasHere==-1)
        {
        PieceWasHere=i;
        }
        else if(PieceWasHere!=-1)
        {
          AnotherPieceWasHere=i;
        }
        
        NumberOfChanges++;
      }
      if(old_boardState[i]==0)
      {
        PieceMovedHere=i;
        NumberOfChanges++;
      }
    }
  }
  
//Outputs to user what move has been made
  //TellUserWhatHappened(PieceWasHere, PieceMovedHere);
  
//Updating the Board Position if all prerequisites are met
  UpdateBoardPosition(PieceWasHere,PieceMovedHere, AnotherPieceWasHere, NumberOfChanges);


  //This block is used to print out the input array for testing purposes
  Serial.println();
  for(int i=0; i<8; i++)
  {
    Serial.print(input[i]); Serial.print("\t");
  }
  Serial.println();
  Serial.println(NumberOfChanges);
  Serial.print("Turn Number= "); Serial.println(NumberOfMovesMade);
  
  Serial.print("PieceWasHere= "); Serial.println(PieceWasHere);
  Serial.print("PieceMovedHere= "); Serial.println(PieceMovedHere);
  Serial.print("AnotherPieceWasHere= "); Serial.println(AnotherPieceWasHere);

}

void TellUserWhatHappened(int PieceWasHere, int PieceMovedHere)
{
  switch (typesAndLocations[PieceWasHere]) 
  {
    case 1: 
      Serial.print("Pawn has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere);
      break;
    case 2: 
      Serial.print("Knight has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere);
      break;
    case 3: 
      Serial.print("Bishop has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere);
      break;
    case 4: 
      Serial.print("Rook has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere); 
      break;
    case 5:
      Serial.print("Queen has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere);
      break;
    case 6:
      Serial.print("King has moved from position ");
      Serial.print(PieceWasHere);
      Serial.print(" to position ");
      Serial.println(PieceMovedHere);
      break;
    default:
      Serial.println("No Moves Made");
      break;
  }
}

void UpdateBoardPosition(int PieceWasHere, int PieceMovedHere,int AnotherPieceWasHere, int NumberOfChanges)
{
  //This block of code is used to update the board when a piece has been moved
  if(PieceWasHere!=-1 & PieceMovedHere!=-1 & NumberOfChanges==2)
  {
    typesAndLocations[PieceMovedHere]=typesAndLocations[PieceWasHere];
    typesAndLocations[PieceWasHere]=0;

    old_boardState[PieceMovedHere]=1;
    old_boardState[PieceWasHere]=0;
    NumberOfMovesMade++;

    if(typesAndLocations[PieceMovedHere]==1 & PieceColors[PieceMovedHere]==1 % (PieceMovedHere==56 | PieceMovedHere==57 | PieceMovedHere==58 | PieceMovedHere==59 | PieceMovedHere==60 | PieceMovedHere==61 | PieceMovedHere==62 | PieceMovedHere==63))
    {
      //send Piece Promotion Flag for White Pieces
      promotedPiece = 1000;
    }
    else if(typesAndLocations[PieceMovedHere]==1 & PieceColors[PieceMovedHere]==2 % (PieceMovedHere==0 | PieceMovedHere==1 | PieceMovedHere==2 | PieceMovedHere==3 | PieceMovedHere==4 | PieceMovedHere==5 | PieceMovedHere==6 | PieceMovedHere==7))
    {
      //Send Piece Promotion Flag for Black Pieces
      promotedPiece = 1001;
    }
    Wire.beginTransmission(engineSA);
    Wire.write(promotedPiece);
    Wire.endTransmission();
  }
  // This block of code is used to update the board when a piece is being captured
  else if(PieceWasHere!=-1 & AnotherPieceWasHere!=-1 & NumberOfChanges==2)
  {
    if((NumberOfMovesMade%2)==0 & PieceColors[PieceWasHere]==1 & PieceColors[AnotherPieceWasHere]==2)
    {
      typesAndLocations[AnotherPieceWasHere]=typesAndLocations[PieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[AnotherPieceWasHere]=1;
      old_boardState[PieceWasHere]=0;
      NumberOfMovesMade++;
    }
    else if((NumberOfMovesMade%2)==0 & PieceColors[AnotherPieceWasHere]==1 & PieceColors[PieceWasHere]==2)
    {
      typesAndLocations[PieceWasHere]=typesAndLocations[AnotherPieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[PieceWasHere]=1;
      old_boardState[AnotherPieceWasHere]=0;
      NumberOfMovesMade++;      
    }
    
    else if((NumberOfMovesMade%2)==1 & PieceColors[PieceWasHere]==1 & PieceColors[AnotherPieceWasHere]==2)
    {
      typesAndLocations[AnotherPieceWasHere]=typesAndLocations[PieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[AnotherPieceWasHere]=1;
      old_boardState[PieceWasHere]=0;
      NumberOfMovesMade++;
    }
    else if((NumberOfMovesMade%2)==1 & PieceColors[AnotherPieceWasHere]==1 & PieceColors[PieceWasHere]==2)
    {
      typesAndLocations[PieceWasHere]=typesAndLocations[AnotherPieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[PieceWasHere]=1;
      old_boardState[AnotherPieceWasHere]=0;
      NumberOfMovesMade++;      
    }
  }
}
*/
