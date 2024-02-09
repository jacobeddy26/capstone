/*
   Taylor Cox
   Chess UI: game and board setup
   31 Jan - 2 Feb 2024
*/

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>

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

#define YP A3  // must be an analog pin, use "An" notation!
#define XM A2  // must be an analog pin, use "An" notation!
#define YM 9   // can be a digital pin
#define XP 8   // can be a digital pin

//Touch For New ILI9341 TP
#define TS_MINX 120
#define TS_MAXX 900

#define TS_MINY 70
#define TS_MAXY 920

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//creates 2 buttons for user to choose side
Elegoo_GFX_Button side_choice[2];
char sidelabels[2][9] = {"  White  ", "  Black  "};
uint16_t sidecolors[2] = {ILI9341_PURPLE, ILI9341_PURPLE};

Elegoo_GFX_Button diff_choice[3];
char difflabels[3][9] = {"   Easy  ", "  Medium ", "   Hard  "};
uint16_t diffcolors[3] = {ILI9341_DARKGREEN, ILI9341_ORANGE, ILI9341_RED};

Elegoo_GFX_Button confirm;

void setup() {
  
  Serial.begin(9600);

  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
     identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  setup_menu();

}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// prints line of text
void status(const __FlashStringHelper *msg) {
  tft.setCursor(10, 10);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(msg);
}

// prints line of text
void status_coord(int x, int y, const __FlashStringHelper *msg) {
  tft.setCursor(x, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(msg);
}

void setup_menu() {

  status(F("Select side:"));

  // button dimensions (makes it easier to read/edit where they're drawn below)
  #define BUTTON_X 120
  #define BUTTON_Y 50
  #define BUTTON_W 80
  #define BUTTON_H 30
  #define BUTTON_SPACING_X 10
  #define BUTTON_SPACING_Y 10
  #define BUTTON_TEXTSIZE 2

  for (uint8_t row=0; row<2; row++) {
   for (uint8_t col=0; col<1; col++) {
     side_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                BUTTON_W, BUTTON_H, ILI9341_WHITE, sidecolors[col + row], ILI9341_WHITE,
                sidelabels[col + row], BUTTON_TEXTSIZE); 
     side_choice[col + row].drawButton();
   }
  }

  status_coord(10, 120, F("Select difficulty:"));

  for (uint8_t row=0; row<3; row++) {
   for (uint8_t col=0; col<1; col++) {
     diff_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                160+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                BUTTON_W, BUTTON_H, ILI9341_WHITE,  diffcolors[col + row], ILI9341_WHITE,
                 difflabels[col + row], BUTTON_TEXTSIZE); 
     diff_choice[col + row].drawButton();
   }
  }

  confirm.initButton(&tft, BUTTON_X, 300, BUTTON_W, BUTTON_H,
                 ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Confirm", 2); 
                     // x, y, w, h, outline, fill, text
  confirm.drawButton();
  bool confirmed = false;

  int chosen_side = 3;
  int chosen_diff = 3;

  while(!confirmed |(chosen_side==3 | chosen_diff==3)) {
    
     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
  
     if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     if (side_choice[0].contains(p.x, p.y)) {
       chosen_side = 0;
       side_choice[0].drawButton(true);  // draw inverted version of button
       side_choice[1].drawButton(false); // make sure other button reverts to original color
     }
     else if (side_choice[1].contains(p.x, p.y)) {
       chosen_side = 1;
       side_choice[1].drawButton(true);  // draw inverted version of button
       side_choice[0].drawButton(false); // make sure other button reverts to original color
     }

     if (diff_choice[0].contains(p.x, p.y)) {
       chosen_diff = 0;
       diff_choice[0].drawButton(true);  // draw inverted version of button
       diff_choice[1].drawButton(false); // make sure other buttons revert to original color
       diff_choice[2].drawButton(false);
     }
     else if (diff_choice[1].contains(p.x, p.y)) {
       chosen_diff = 1;
       diff_choice[1].drawButton(true);  // draw inverted version of button
       diff_choice[0].drawButton(false); // make sure other buttons revert to original color
       diff_choice[2].drawButton(false);
     }
     else if (diff_choice[2].contains(p.x, p.y)) {
       chosen_diff = 2;
       diff_choice[2].drawButton(true);  // draw inverted version of button
       diff_choice[0].drawButton(false); // make sure other buttons revert to original color
       diff_choice[1].drawButton(false);
     }

     if(chosen_side!=3 && chosen_diff!=3 && confirm.contains(p.x, p.y)) {
         confirmed = true;
         confirm.drawButton(true);  // draw inverted version of buttons
     }

  }

  tft.fillScreen(BLACK);
  setup_board();
}

void setup_board() {
  
  status(F("Please confirm the  board has been set  up according to the manual."));
  
  // button dimensions (makes it easier to read/edit where they're drawn below)
  #define BUTTON_X 120
  #define BUTTON_Y 50
  #define BUTTON_W 80
  #define BUTTON_H 30
  #define BUTTON_SPACING_X 10
  #define BUTTON_SPACING_Y 10
  #define BUTTON_TEXTSIZE 2

  confirm.initButton(&tft, BUTTON_X, 200, BUTTON_W, BUTTON_H,
                 ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Confirm", 2); 
                     // x, y, w, h, outline, fill, text
  confirm.drawButton();
  bool confirmed = false;

  while(!confirmed) {
    
     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
  
     if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     if(confirm.contains(p.x, p.y)) {
         confirmed = true;
         confirm.drawButton(true);  // draw inverted version of buttons
     }
  }

  tft.fillScreen(BLACK);
  // call to start game here
}

void loop() {}