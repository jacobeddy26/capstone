
#include "Wire.h"
#include <Adafruit_CAP1188.h>

Adafruit_CAP1188 cap1 = Adafruit_CAP1188();
Adafruit_CAP1188 cap2 = Adafruit_CAP1188();
Adafruit_CAP1188 cap3 = Adafruit_CAP1188();
Adafruit_CAP1188 cap4 = Adafruit_CAP1188();
Adafruit_CAP1188 cap5 = Adafruit_CAP1188();
Adafruit_CAP1188 cap6 = Adafruit_CAP1188();
Adafruit_CAP1188 cap7 = Adafruit_CAP1188();
Adafruit_CAP1188 cap8 = Adafruit_CAP1188();

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller
#define TCAADDR 0x70

#define pawn 1
#define knight 2
#define bishop 3
#define rook 4
#define queen 5
#define king 6

int promoteFlag;
bool promoteWhite=false;
bool promoteBlack=false
int dataIn;

//This array contains the binary version of the current board that the inputs from the capacitive touch sensors will be compared against to see if a move has been made
int old_boardState[64]={1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// piece type and location
//0=none, 1=pawn, 2=knight, 3=bishop, 4=rook, 5=queen, 6=king
int typesAndLocations[64]={6, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Counter recording how many moves have been made
int NumberOfMovesMade=1;

//Array containing color of each piece
//0=no piece, 1=white, 2=black
int PieceColors[64]={1, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

//Encoder Array, Contains strings of ID of each Square
String Encoder[64]={"a1","b1","c1","d1","e1","f1","g1","h1","a2","b2","c2","d2","e2","f2","g2","h2","a3","b3","c3","d3","e3","f3","g3","h3","a4","b4","c4","d4","e4","f4","g4","h4","a5","b5","c5","d5","e5","f5","g5","h5","a6","b6","c6","d6","e6","f6","g6","h6","a7","b7","c7","d7","e7","f7","g7","h7","a8","b8","c8","d8","e8","f8","g8","h8"};

// standard Arduino setup()
void setup()
{
    Serial.begin(115200);
    Wire.begin(boardSA); // Initialize I2C communication with set slave address
    Wire.onReceive(receiveEvent);
    

    //Code to Test that Multiplexer and Cap Boards are set up properly
    
    Serial.println("\nTCAScanner ready!");

    for (uint8_t t=0; t<8; t++) {
      tcaselect(t);
      Serial.print("TCA Port #"); Serial.println(t);

      for (uint8_t addr = 0; addr<=127; addr++) {
        if (addr == TCAADDR) continue;

        Wire.beginTransmission(addr);
        if (!Wire.endTransmission()) {
          Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
        }
      }
    }
    Serial.println("\ndone");

    //Checking the initialization of each capacitive touch sensor 
    tcaselect(0);
    if (!cap1.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(1);
    if (!cap2.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(2);
    if (!cap3.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(3);
    if (!cap4.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(4);
    if (!cap5.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(5);
    if (!cap6.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(6);
    if (!cap7.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    tcaselect(7);
    if (!cap8.begin()) 
    {
    Serial.println("CAP1188 not found");
    while (1);
    }
    Serial.println("CAP1188 found!");
}

void loop() 
{
  //Pulling data from each capacitive touch sensor
  tcaselect(0);
  uint8_t touched1 = cap1.touched();
  
  tcaselect(1);
  uint8_t touched2 = cap2.touched();
  
  tcaselect(2);
  uint8_t touched3 = cap3.touched();
  
  tcaselect(3);
  uint8_t touched4 = cap4.touched();
  
  tcaselect(4);
  uint8_t touched5 = cap5.touched();

  tcaselect(5);
  uint8_t touched6 = cap6.touched();

  tcaselect(6);
  uint8_t touched7 = cap7.touched();

  tcaselect(7);
  uint8_t touched8 = cap8.touched();
  
  int input[64];
  int PieceWasHere=-1;
  int AnotherPieceWasHere=-1; //Variable used when capturing pieces
  int PieceMovedHere=-1;
  int NumberOfChanges=0;
  String MoveMessage="none";

  //This block creates an input array from the sensor inputs, and compares the input array to the current boardstate known by the computer. 
  //By comparing these arrays, the computer can identify moves made by the user when there are differences between the arrays.
  for (uint8_t i=0; i<8; i++) 
  {
    if (touched1 & (1 << i)) 
    {
      input[i]=1;
    }
    else
    {
      input[i]=0;
    }
    
    if (touched2 & (1 << i)) 
    {
      input[i+8]=1;
    }
    else
    {
      input[i+8]=0;
    }
    
    if (touched3 & (1 << i)) 
    {
      input[i+16]=1;
    }
    else
    {
      input[i+16]=0;
    }
    
    if (touched4 & (1 << i)) 
    {
      input[i+24]=1;
    }
    else
    {
      input[i+24]=0;
    }
    
    if (touched5 & (1 << i)) 
    {
      input[i+32]=1;
    }
    else
    {
      input[i+32]=0;
    }
    if (touched6 & (1 << i)) 
    {
      input[i+40]=1;
    }
    else
    {
      input[i+40]=0;
    }
    if (touched7 & (1 << i)) 
    {
      input[i+48]=1;
    }
    else
    {
      input[i+48]=0;
    }
    if (touched8 & (1 << i)) 
    {
      input[i+56]=1;
    }
    else
    {
      input[i+56]=0;
    }
    
  }


  for (uint8_t i=0; i<64; i++)
  {
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
  
  //Updating the Board Position if all prerequisites are met
  UpdateBoardPosition(PieceWasHere,PieceMovedHere, AnotherPieceWasHere, NumberOfChanges, MoveMessage);


  Serial.println();
  for (uint8_t i=0; i<64; i++)
  {
    if(i==8 || i==16 || i==24 || i==32 || i==40 || i==48 || i==56)
    {
      Serial.println();
    }
    Serial.print(input[i]);
    Serial.print(" ");
    
  }
  Serial.println();
  Serial.println(NumberOfChanges);
  Serial.print("Turn Number= "); Serial.println(NumberOfMovesMade);
  
  Serial.print("PieceWasHere= "); Serial.println(PieceWasHere);
  Serial.print("PieceMovedHere= "); Serial.println(PieceMovedHere);
  Serial.print("AnotherPieceWasHere= "); Serial.println(AnotherPieceWasHere);
  Serial.println();
  delay(3000);

}


void UpdateBoardPosition(int PieceWasHere, int PieceMovedHere,int AnotherPieceWasHere, int NumberOfChanges, String MoveMessage)
{
  //This block of code is used to update the board when a piece has been moved
  if(PieceWasHere!=-1 & PieceMovedHere!=-1 & NumberOfChanges==2)
  {
    typesAndLocations[PieceMovedHere]=typesAndLocations[PieceWasHere];
    typesAndLocations[PieceWasHere]=0;

    old_boardState[PieceMovedHere]=1;
    old_boardState[PieceWasHere]=0;

    PieceColors[PieceMovedHere]=PieceColors[PieceWasHere];
    PieceColors[PieceWasHere]=0;

    MoveMessage=Encoder[PieceWasHere]+Encoder[PieceMovedHere];
    NumberOfMovesMade++;

    //Castling Code
    //White Queenside Castle
    if(typesAndLocations[PieceMovedHere]==6 & PieceWasHere==4 & PieceMovedHere==2)
    {
      typesAndLocations[0]=0;
      typesAndLocations[3]=4;
      typesAndLocations[4]=0;
      typesAndLocations[2]=6;

      PieceColors[0]=0;
      PieceColors[3]=1;
      PieceColors[4]=0;
      PieceColors[2]=1;
    }
    //White Kingside Castle
    else if(typesAndLocations[PieceMovedHere]==6 & PieceWasHere==4 & PieceMovedHere==6)
    {
      typesAndLocations[7]=0;
      typesAndLocations[5]=4;
      typesAndLocations[4]=0;
      typesAndLocations[6]=6;

      PieceColors[7]=0;
      PieceColors[5]=1;
      PieceColors[4]=0;
      PieceColors[6]=1;
    }
    else if(typesAndLocations[PieceMovedHere]==6 & PieceWasHere==60 & PieceMovedHere==58)
    {
      typesAndLocations[60]=0;
      typesAndLocations[58]=6;
      typesAndLocations[56]=0;
      typesAndLocations[59]=4;

      PieceColors[60]=0;
      PieceColors[58]=1;
      PieceColors[56]=0;
      PieceColors[59]=1;
    }
    else if(typesAndLocations[PieceMovedHere]==6 & PieceWasHere==60 & PieceMovedHere==62)
    {
      typesAndLocations[60]=0;
      typesAndLocations[62]=6;
      typesAndLocations[63]=0;
      typesAndLocations[61]=4;

      PieceColors[60]=0;
      PieceColors[62]=1;
      PieceColors[63]=0;
      PieceColors[61]=1;
    }    

    //Piece Promotion Code 
    /*
    if(typesAndLocations[PieceMovedHere]==1 & PieceColors[PieceMovedHere]==1 % (PieceMovedHere==56 | PieceMovedHere==57 | PieceMovedHere==58 | PieceMovedHere==59 | PieceMovedHere==60 | PieceMovedHere==61 | PieceMovedHere==62 | PieceMovedHere==63))
    {
      //send Piece Promotion Flag for White Pieces
      promoteWhite=true;
      promote(promoteWhite,promoteBlack);
    }
    else if(typesAndLocations[PieceMovedHere]==1 & PieceColors[PieceMovedHere]==2 % (PieceMovedHere==0 | PieceMovedHere==1 | PieceMovedHere==2 | PieceMovedHere==3 | PieceMovedHere==4 | PieceMovedHere==5 | PieceMovedHere==6 | PieceMovedHere==7))
    {
      //Send Piece Promotion Flag for Black Pieces
      promoteBlack=true;
      promote(promoteWhite,promoteBlack);
    }

    */
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

      PieceColors[AnotherPieceWasHere]=PieceColors[PieceWasHere];
      PieceColors[PieceWasHere]=0;
      NumberOfMovesMade++;
    }
    else if((NumberOfMovesMade%2)==0 & PieceColors[AnotherPieceWasHere]==1 & PieceColors[PieceWasHere]==2)
    {
      typesAndLocations[PieceWasHere]=typesAndLocations[AnotherPieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[PieceWasHere]=1;
      old_boardState[AnotherPieceWasHere]=0;

      PieceColors[PieceWasHere]=PieceColors[AnotherPieceWasHere];
      PieceColors[PieceWasHere]=0; 
      NumberOfMovesMade++;      
    }
    
    else if((NumberOfMovesMade%2)==1 & PieceColors[PieceWasHere]==1 & PieceColors[AnotherPieceWasHere]==2)
    {
      typesAndLocations[AnotherPieceWasHere]=typesAndLocations[PieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[AnotherPieceWasHere]=1;
      old_boardState[PieceWasHere]=0;

      PieceColors[AnotherPieceWasHere]=PieceColors[PieceWasHere];
      PieceColors[PieceWasHere]=0;
      NumberOfMovesMade++;
    }
    else if((NumberOfMovesMade%2)==1 & PieceColors[AnotherPieceWasHere]==1 & PieceColors[PieceWasHere]==2)
    {
      typesAndLocations[PieceWasHere]=typesAndLocations[AnotherPieceWasHere];
      typesAndLocations[PieceWasHere]=0;

      old_boardState[PieceWasHere]=1;
      old_boardState[AnotherPieceWasHere]=0;
    
      PieceColors[PieceWasHere]=PieceColors[AnotherPieceWasHere];
      PieceColors[PieceWasHere]=0;
      NumberOfMovesMade++;      
    }
    
    MoveMessage=Encoder[PieceWasHere]+Encoder[PieceMovedHere];
  }

}

void tcaselect(uint8_t i) 
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();  
}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
   if(Wire.available()) {
      dataIn = Wire.read();
      selectedPiece(dataIn);
  }
}

void promote(bool flagWhite, bool flagBlack) {
    if(flagWhite)
    {
      //send Piece Promotion Flag for White Pieces
      promoteFlag = 1000;
      Serial.println("White is Promoting!");
    }
    else if(flagBlack) {
      //Send Piece Promotion Flag for Black Pieces
      promoteFlag = 1001;
      Serial.println("Black is Promoting!");
    }
    Wire.beginTransmission(engineSA);
    Wire.write(promoteFlag);
    Wire.endTransmission();
    promoteBlack=false;
    promoteWhite=false;
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
