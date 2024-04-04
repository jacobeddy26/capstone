/*
   Taylor Cox
   Chess UI: in-game options menu
   30 Jan - 26 Mar 2024
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

//creates in-game options buttons for user
Elegoo_GFX_Button ingame[3];
char ingame_labels[3][12] = {"Hints", "Moves", "Forfeit"};
uint16_t ingame_colors[3] = {ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK};
bool hints_on = false; // must be global as other screens may be called in the meantime
bool possible_moves_on = false;
bool user_forfeit = false;

Elegoo_GFX_Button yn[2];
char yn_labels[2][12] = {" Confirm    ", "  Back      "};
uint16_t yn_colors[2] = {ILI9341_BLACK, ILI9341_BLACK};

void setup() {
  
  Serial.begin(9600);

  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
     identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(2);

  ingame_menu(); // HARDWIRED CALL FOR TESTING ONLY, should be called during game whenever other screens are not in use

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

void forfeit_confirm() {
  bool decided = false;
  
  tft.fillScreen(0x03AB);
  status(F("Are you sure you   want to forfeit the game?"));

  // button dimensions (makes it easier to read/edit where they're drawn below)
  #define BUTTON_X 120
  #define BUTTON_Y 100
  #define BUTTON_W 100
  #define BUTTON_H 30
  #define BUTTON_SPACING_X 20
  #define BUTTON_SPACING_Y 20
  #define BUTTON_TEXTSIZE 2

  for (uint8_t row=0; row<2; row++) {
  for (uint8_t col=0; col<1; col++) {
    yn[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
              BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
              BUTTON_W, BUTTON_H, ILI9341_WHITE, yn_colors[col + row], ILI9341_WHITE,
              yn_labels[col + row], BUTTON_TEXTSIZE); 
    yn[col + row].drawButton();
    }
  }

  while(!decided) {

     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);

    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
      p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
    }

    if (yn[0].contains(p.x, p.y)) { // forfeit confirmed
      yn[0].drawButton(true); // invert button colors
      decided = true;
      tft.fillScreen(BLACK);
      // call game over screen
    }
    if (yn[1].contains(p.x, p.y)) { // forfeit denied
      yn[1].drawButton(true); // invert button colors
      decided = true;
      tft.fillScreen(BLACK);
      user_forfeit = false;
      setup();
      loop();
    }
  }
  
}

void ingame_menu() {
  
  tft.fillScreen(ILI9341_DARKGREEN);
  status(F("Game options"));

  // button dimensions (makes it easier to read/edit where they're drawn below)
  #define BUTTON_X 120
  #define BUTTON_Y 100
  #define BUTTON_W 100
  #define BUTTON_H 30
  #define BUTTON_SPACING_X 20
  #define BUTTON_SPACING_Y 20
  #define BUTTON_TEXTSIZE 2
  
  for (uint8_t row=0; row<3; row++) {
   for (uint8_t col=0; col<1; col++) {
     ingame[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
                BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y),    // x, y, w, h, outline, fill, text
                BUTTON_W, BUTTON_H, ILI9341_WHITE, ingame_colors[col + row], ILI9341_WHITE,
                ingame_labels[col + row], BUTTON_TEXTSIZE); 
     ingame[col + row].drawButton();
   }
  }
  
  if(hints_on) // corrects hint button color if returning to menu from another screen
     ingame[0].drawButton(true);  // draw inverted version of button

  if(possible_moves_on)
     ingame[1].drawButton(true);

  while(!user_forfeit) {
     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
  
     if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }
     
     if(ingame[0].justReleased()) { // makes it only redraw if button was just released (will flash otherwise)
          if(hints_on)
             ingame[0].drawButton(true);  // draw inverted version of button
          else
             ingame[0].drawButton(false); // draw regular button
     }

     if(ingame[1].justReleased()) {
          if(possible_moves_on)
             ingame[1].drawButton(true);  // draw inverted version of button
          else
             ingame[1].drawButton(false); // draw regular button
     }

     if (ingame[0].contains(p.x, p.y)) {
        ingame[0].press(true);  // tell the button it is pressed
        hints_on = !hints_on; // toggle hint variable
        delay(100);
     }
     else if (ingame[1].contains(p.x, p.y)) {
        ingame[1].press(true);  // tell the button it is pressed
        possible_moves_on = !possible_moves_on; // toggle possible moves variable
        delay(100);
     }
     else if (ingame[2].contains(p.x, p.y)) {
        ingame[2].press(true);  // tell the button it is pressed
        ingame[2].drawButton(true);  // draw inverted version of button
        user_forfeit = true;
        delay(100); //time for user to see button invert
        tft.fillScreen(BLACK);
        forfeit_confirm();
     }
     else { // in testing, this seemed necessary for delay in this particular case, idk why
        ingame[0].press(false);
        ingame[1].press(false);
        ingame[2].press(false);
     }
  }
}

void loop() {}
