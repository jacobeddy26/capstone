// Code for Engine Controller & LCD I2C Promotion Test
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <Wire.h>
#include <stdint.h>

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Color definitions
#define ILI9341_BLACK       0x0000      /*   0,   0,   0 */
#define ILI9341_NAVY        0x000F      /*   0,   0, 128 */
#define ILI9341_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9341_DARKCYAN    0x03EF      /*   0, 128, 128 */
#define ILI9341_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9341_PURPLE      0x780F      /* 128,   0, 128 */
#define ILI9341_OLIVE       0x7BE0      /* 128, 128,   0 */
#define ILI9341_LIGHTGREY   0xC618      /* 192, 192, 192 */
#define ILI9341_DARKGREY    0x7BEF      /* 128, 128, 128 */
#define ILI9341_BLUE        0x001F      /*   0,   0, 255 */
#define ILI9341_GREEN       0x07E0      /*   0, 255,   0 */
#define ILI9341_CYAN        0x07FF      /*   0, 255, 255 */
#define ILI9341_RED         0xF800      /* 255,   0,   0 */
#define ILI9341_MAGENTA     0xF81F      /* 255,   0, 255 */
#define ILI9341_YELLOW      0xFFE0      /* 255, 255,   0 */
#define ILI9341_WHITE       0xFFFF      /* 255, 255, 255 */
#define ILI9341_ORANGE      0xFD20      /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5      /* 173, 255,  47 */
#define ILI9341_PINK        0xF81F

/******************* UI details */
#define BUTTON_X 120
#define BUTTON_Y 100
#define BUTTON_W 80
#define BUTTON_H 30
#define BUTTON_SPACING_X 20
#define BUTTON_SPACING_Y 20
#define BUTTON_TEXTSIZE 2

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

#define MINPRESSURE 10
#define MAXPRESSURE 1000

#define engineSA 1   // Slave Address for the chess engine controller
#define boardSA 2    // Slave Address for the board/LCD controller
#define scaraSA 3    // Slave Address for the SCARA controller

# define whitePromotion 1000
# define blackPromotion 1001


#define knight 8
#define bishop 12
#define rook 16
#define queen 24
/////////////////////////////////////////////////////////////////////////////

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//creates 4 buttons for user to choose what to promote to
Elegoo_GFX_Button promo_choice[4];
char promolabels[4][8] = {"Queen", "Rook", "Knight", "Bishop"};
uint16_t promocolors[4] = {ILI9341_PURPLE, ILI9341_PURPLE, ILI9341_PURPLE, ILI9341_PURPLE};
bool promo_done = false;
int dataIn;

bool userPromoteFlag = false;
bool computerPromoteFlag = false;

int promotedPiece = 0;

void setup() {
  Serial.begin(9600);
  Wire.begin(engineSA); // Initialize I2C communication as master
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
   
  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
    identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);
}

void loop() {
  /*
  char dataToSend[] = "b2b40"; // Char array of length 5
  Wire.beginTransmission(scaraSA); // Slave address (change as needed)
  Wire.write(dataToSend, 5); // Send the char array
  Wire.endTransmission();
  delay(1000); // Wait for a second before sending again
  */
}

// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
   userPromoteFlag = false;
   computerPromoteFlag = false;
   if(Wire.available()) {
      dataIn = Wire.read();
      Serial.print("dataIn: ");
      Serial.print(dataIn);
      Serial.print("\n");
   }

  if (dataIn == 0)
      userPromoteFlag = true;
  else if (dataIn == 1)
      computerPromoteFlag = true; 
}

// function that executes whenever data is requested by master
// this function is registered as an event, see setup()
void requestEvent() {
  promotedPiece = 0;
  if(userPromoteFlag==true) {
    userPromotionPrompt();
  } else if (computerPromoteFlag==true) {
    // computerPromotionPrompt();
  }
  delay(500);
  Serial.print("promotedPiece: ");
  Serial.print(promotedPiece);
  Serial.print("\n");
  Wire.write(promotedPiece);
}

// prints line of text
void status(const __FlashStringHelper *msg) {
  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(msg);
}

void userPromotionPrompt() {
  tft.fillScreen(BLACK);
  status(F("Select which piece to promote to:"));

  for (uint8_t row=0; row<4; row++) {
    for (uint8_t col=0; col<1; col++) {
      promo_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                  BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, promocolors[col + row], ILI9341_WHITE,
                  promolabels[col + row], BUTTON_TEXTSIZE); 
      promo_choice[col + row].drawButton();
    }
  }

  while(promo_done==false) {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);
  
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }

    //check if all buttons were pressed
    for (uint8_t b=0; b<4; b++) {
      if (promo_choice[b].contains(p.x, p.y)) {
          promo_choice[b].press(true);  // tell the button it is pressed
        promo_choice[b].drawButton(true);  // draw inverted version of button
        delay(100); //time for user to see button invert
        tft.fillScreen(BLACK);
        switch (b) {
          case 0:
            promotedPiece=queen;
            promo_done = true; 
            status(F("Promoted to queen.")); 
            break;
          case 1:
            promotedPiece=rook;
            promo_done = true;
            status(F("Promoted to rook."));
            break;
          case 2:
            promotedPiece=knight;
            promo_done = true;
            status(F("Promoted to knight."));
            break;
          case 3:
            promotedPiece=bishop;
            promo_done = true;
            status(F("Promoted to bishop."));
            break;
        }
      }
      delay(100); // UI debouncing
    }
  }
}
/*
int computerPromotionPrompt() {
   int piece;

   for (uint8_t row=0; row<4; row++) {
      for (uint8_t col=0; col<1; col++) {
      promo_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                  BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                  BUTTON_W, BUTTON_H, ILI9341_WHITE, promocolors[col + row], ILI9341_WHITE,
                  promolabels[col + row], BUTTON_TEXTSIZE); 
      promo_choice[col + row].drawButton();
      }
  }

   while(!promo_done) {

     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
  
     if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     //check if all buttons were pressed
     for (uint8_t b=0; b<4; b++) {
       if (promo_choice[b].contains(p.x, p.y)) {
         promo_choice[b].press(true);  // tell the button it is pressed
         promo_done = true;
         promo_choice[b].drawButton(true);  // draw inverted version of button
         delay(100); //time for user to see button invert
         tft.fillScreen(BLACK);
         switch (b) {
           case 0:
            piece = queen; 
            status(F("Promoted to queen.")); 
            break;
           case 1:
            piece = rook; 
            status(F("Promoted to rook.")); 
            break;
           case 2:
            piece = knight; 
            status(F("Promoted to knight.")); 
            break;
           case 3:
            piece = bishop; 
            status(F("Promoted to bishop.")); 
            break;
           default: piece = 0; break;
         }
         Serial.println(piece);
       }
    delay(100); // UI debouncing
  }
  return piece;
}
*/