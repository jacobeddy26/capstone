/*
   Taylor Cox
   Display computer promotion on UI
   23 Jan 2024
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

/******************* UI details */
#define BUTTON_X 120
#define BUTTON_Y 160
#define BUTTON_W 200
#define BUTTON_H 320

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

Elegoo_GFX_Button screen;
bool cpu_promo_done = false;

void setup() {
  
  Serial.begin(9600);

  tft.reset();
  uint16_t identifier = tft.readID();
  if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
     identifier = 0x9341;
  tft.begin(identifier);
  tft.setRotation(2);
  tft.fillScreen(BLACK);

  int cpu_promo_to = 3;
  status_coord(10, 10, F("The computer is promoting a pawn. Please replace it with an available "));
  if(cpu_promo_to==0)
     status_coord(130, 58, F("queen."));
  else if(cpu_promo_to==1)
     status_coord(130, 58, F("rook."));
  else if(cpu_promo_to==2)
     status_coord(130, 58, F("knight."));
  else if(cpu_promo_to==3)
     status_coord(130, 58, F("bishop."));

  screen.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                 ILI9341_WHITE, ILI9341_DARKGREY, ILI9341_WHITE, "Done", 2); 
                     // x, y, w, h, outline, fill, text
  screen.drawButton();

}

#define MINPRESSURE 10
#define MAXPRESSURE 1000

// prints line of text
void status_coord(int x, int y, const __FlashStringHelper *msg) {
  tft.setCursor(x, y);
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(2);
  tft.print(msg);
}

void loop() {

  while(!cpu_promo_done) {

     digitalWrite(13, HIGH);
     TSPoint p = ts.getPoint();
     digitalWrite(13, LOW);
  
     pinMode(XM, OUTPUT);
     pinMode(YP, OUTPUT);
  
     if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
       p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     if(screen.contains(p.x, p.y)) {
         cpu_promo_done = true;
         screen.press(true);
         tft.fillScreen(BLACK);
     }
     else
         screen.press(false);  // tell the button it is NOT pressed
  }

  delay(100); // UI debouncing

}
