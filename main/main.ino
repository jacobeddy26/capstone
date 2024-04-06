/***************************************************************************/
/* Ported chess program to Arduino MEGA by Diego Cueva. Based on:          */
/*                   micro-Max,       version 4.8                          */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/* Port to Atmel ATMega644 and AVR GCC, by Andre Adrian                    */
/* Quito junio 2014        Ecuador                                         */
/***************************************************************************/
#include <Wire.h>
#include <LiquidCrystal.h>
#include "Chessuino.h"
#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <TouchScreen.h>
#include <avr/wdt.h>

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

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
TouchScreen ts = TouchScreen(XP, YP, XM, YM, 300);

//creates 2 buttons for user to choose side
Elegoo_GFX_Button side_choice[2];
char sidelabels[2][9] = {"  White  ", "  Black  "};
uint16_t sidecolors[2] = {ILI9341_PURPLE, ILI9341_PURPLE};

Elegoo_GFX_Button diff_choice[3];
char difflabels[3][9] = {"  Easy  \0", "  Medium ","  Hard  \0"};
uint16_t diffcolors[3] = {ILI9341_DARKGREEN, ILI9341_ORANGE, ILI9341_RED};

//creates in-game options buttons for user
Elegoo_GFX_Button ingame[3];
char ingame_labels[3][12] = {"Hints", "Moves", "Forfeit"};
uint16_t ingame_colors[3] = {ILI9341_BLACK, ILI9341_BLACK, ILI9341_BLACK};
bool hints_on = false; // must be global as other screens may be called in the meantime
bool possible_moves_on = false;
bool user_forfeit = false;

// creates 2 buttons to confirm 
Elegoo_GFX_Button yn[2];
char yn_labels[2][12] = {" Confirm    ", "  Back      "};
uint16_t yn_colors[2] = {ILI9341_BLACK, ILI9341_BLACK};

//creates 4 buttons for user to choose what to promote to
Elegoo_GFX_Button promo_choice[4];
char promolabels[4][8] = {"  Queen ", "   Rook ", "  Knight", " Bishop "};
uint16_t promocolors[4] = {ILI9341_PURPLE, ILI9341_PURPLE, ILI9341_PURPLE, ILI9341_PURPLE};

Elegoo_GFX_Button confirm;
Elegoo_GFX_Button cont;
Elegoo_GFX_Button start;
Elegoo_GFX_Button done;

char promo_to;
bool promo_done = false;
bool cpu_promo_done = false;

//global variables that hold binary digits for source and destination squares
bool d5src, d4src, d3src, d2src, d1src, d0src;
bool d5dst, d4dst, d3dst, d2dst, d1dst, d0dst;

// Define chessboard dimensions
const int BOARD_SIZE = 8;

// Define piece constants
#define EMPTY 0
#define PAWN 1
#define KNIGHT 2
#define BISHOP 3
#define ROOK 4
#define QUEEN 5
#define KING 6

// Define piece colors
#define WHITE 0
#define BLACK 1

// Define piece structures
struct Piece {
   int type;
   int color;
};

// Define chessboard
Piece chessboard[BOARD_SIZE][BOARD_SIZE];

#define pawn 1
#define knight 2
#define bishop 3
#define rook 4
#define queen 5
#define king 6

#define scaraSA 3                                // Slave Address for SCARA controller
#define boardSA 2                                // Slave Address for Board/LCD controller
#define engineSA 1                               // Slave Address for Chess Engine controller

char bestMove[5] = "e2e4\0"; // Initialize best move globally
char inputMove[5] = "none\0";
char outputMove[6];

bool isCastle, isCapture;
bool moveReceived = false;
bool isWhiteSelected = true;

void setup(){
   Wire.begin(engineSA);
   Wire.onReceive(receiveEvent);
   Serial.begin(115200);
   Serial.println("  *** CHESSuino ***");

   tft.reset();
   uint16_t identifier = tft.readID();
   if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
      identifier = 0x9341;
   tft.begin(identifier);
   tft.setRotation(2);
   tft.fillScreen(BLACK);
 
   lastH[0] = 0;
   pinMode(13, OUTPUT);

   setup_menu();
   if (isWhiteSelected==false)
   {
      strcpy(bestMove,"c7c5\0");
   }
   setup_board();
}

void loop(){
   int r;
   delay(500);
   // Calculate and output human's best move
   UserBestMove();
   delay(500);

   // Take move from human
   x1=x2=y1=y2=-1;
   takeMove();
   /* Turn for ARDUINO */
    
   K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;      /* parse entered move */
   N=0;
   //T=0x3F;                                   /* T=Computer Play strength */
   bkp();                                    /* Save the board just in case */    
   r = D(-I,I,Q,O,1,3);                      /* Check & do the human movement */
   if( !(r>-I+1) ){
      // Lose
      game_over();
   } else {
      // Move Not Found!
      illegal_move_alert();
   }
   if(k == 0x10){                            /* The flag turn must change to 0x08 */
      return;
   }
  
   // Input human move
   strcpy(lastH, c);                         /* Valid human movement */

   mn++;                                     /* Next move */
   searchDataBase();                         /* Search in database */
   if(c[0]){                                 /* Movement found */
      K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;  /* parse move found */
      N=0;
      //T=0x3F;                               /* T=Computer Play strength */
      r = D(-I,I,Q,O,1,3);                  /* Check & do*/
      if( !(r>-I+1) ) game_over();
      if(k == 0x08){
         //ERR DB
         game_over();
      }
        
      strcpy(lastM, c);                         /* Valid ARDUINO movement */

      outputMove[0]='?';
      outputMove[1]=c[1];
      outputMove[2]=c[2];
      outputMove[3]=c[3];
      outputMove[4]=c[4];
      // Throw a flag if the calculated move is a capture or castle
      if (isCapture) {                             // Set flag for capture
         outputMove[0]='x';
         Wire.beginTransmission(scaraSA);          // Transmit capture move
         Wire.write(outputMove,6); 
         Wire.endTransmission();                             
      } else if (isCastle) {                       // Set flag for castle
         outputMove[0]='o';
         Wire.beginTransmission(scaraSA);          // Transmit castle move
         Wire.write(outputMove,6); 
         Wire.endTransmission();                   
      } else {
         Wire.beginTransmission(scaraSA);          // Transmit normal move
         Wire.write(outputMove,6); 
         Wire.endTransmission();
      }

      delay(1000);        
      return;
   }
    
   K=I;
   N=0;
   //T=0x3F;                                 /* T=Computer Play strength */
   r = D(-I,I,Q,O,1,3);                      /* Think & do computer's move*/    
   if( !(r>-I+1) ){
      // Lose*
      game_over();
   }
    
   if(k == 0x08){                            /* Some times the algorithm do not */
      // ERR 3                               /* execute the move and do not change the turn flag */
      game_over();                            /* 1. b1c3  c7c5?       2. f2f4? */
   }

   strcpy(lastM, c);                         /* Valid ARDUINO movement */
   
   outputMove[0]='?';
   outputMove[1]=c[1];
   outputMove[2]=c[2];
   outputMove[3]=c[3];
   outputMove[4]=c[4];
   // Throw a flag if the calculated move is a capture or castle
   if (isCapture) {                             // Set flag for capture
      outputMove[0]='x';
      Wire.beginTransmission(scaraSA);          // Transmit capture move
      Wire.write(outputMove,6); 
      Wire.endTransmission();                             
   } else if (isCastle) {                       // Set flag for castle
      outputMove[0]='o';
      Wire.beginTransmission(scaraSA);          // Transmit castle move
      Wire.write(outputMove,6); 
      Wire.endTransmission();                   
   } else {
      Wire.beginTransmission(scaraSA);          // Transmit normal move
      Wire.write(outputMove,6); 
      Wire.endTransmission();
   }

   strcpy(c, "a1a1");                        /* Execute a invalid move to check score again */
   r = D(-I,I,Q,O,1,3);
   if( !(r>-I+1) ){
      game_over();
   }
   if(k == 0x08){                            /* Some times the algorithm do not */
      // ERR 3                               /* execute the move and do not change the turn flag */
      game_over();                            /* 1. b1c3  c7c5?       2. f2f4? */
   }    
   delay(500);
}

void takeMove(){  
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

   while(moveReceived==false) {
      ingame_menu();

      if (inputMove[0] != 'n')
      {
         strcpy(c,inputMove);
         strcpy(inputMove,"none\0");
         moveReceived=false;
         break;
      }
   }
}

/* User interface routines */
void myputchar(char c) {
   Serial.print(c);
}

void myputs(const char *s) {
   while(*s) myputchar(*s++);
   myputchar('\n');
}

char mygetchar(void) {
   while(Serial.available() <= 0);
   return (char)Serial.read();
}

unsigned short myrand(void) {
   unsigned short r = (unsigned short)(seed%MYRAND_MAX);
   return r=((r<<11)+(r<<7)+r)>>1;
}
/* recursive minimax search */
/* (q,l)=window, e=current eval. score, */
/* E=e.p. sqr.z=prev.dest, n=depth; return score */
short D(short q, short l, short e, unsigned char E, unsigned char z, unsigned char n){
   short m,v,i,P,V,s;
   unsigned char t,p,u,x,y,X,Y,H,B,j,d,h,F,G,C;
   signed char r;
   isCapture = false;
   isCastle = false;

   if (++Z>30) {                                     /* stack underrun check */
      breakpoint=1;               /* AVR Studio 4 Breakpoint for stack underrun */
      myputchar('u');
      --Z;return e;                                    
   }
  
   digitalWrite(13, (ledv++)%8?0:1);
 
   q--;                                          /* adj. window: delay bonus */
   k^=24;                                        /* change sides             */
   d=Y=0;                                        /* start iter. from scratch */
   X=myrand()&~M;                                /* start at random field    */
   W(d++<n||d<3||                                /* iterative deepening loop */
      z&K==I&&(N<T&d<98||                         /* root: deepen upto time   */
      (K=X,L=Y&~M,d=3)))                          /* time's up: go do best    */
   {x=B=X;                                       /* start scan at prev. best */
      h=Y&S;                                       /* request try noncastl. 1st*/
      P=d<3?I:D(-l,1-l,-e,S,0,d-3);                /* Search null move         */
      m=-P<l|R>35?d>2?-I:e:-P;                     /* Prune or stand-pat       */
      ++N;                                         /* node count (for timing)  */
      do{
         if (p == 6 && abs(y - x) == 2) {            // If the moving piece is the king and the move is a two-square move
            isCastle = true;                        // Set flag for castle
         }
         u=b[x];                                     /* scan board looking for   */
         if(u&k){                                    /*  own piece (inefficient!)*/
            r=p=u&7;                                   /* p = piece type (set r>0) */
            j=o[p+16];                                 /* first step vector f.piece*/
            W(r=p>2&r<0?-r:-o[++j])                    /* loop over directions o[] */
            // Check if move is a capture
            if (i < 0) {                               /* Capture move */
               isCapture = true;                       /* Set flag for capture */
            }
            {A:                                        /* resume normal after best */
               y=x;F=G=S;                                /* (x,y)=move, (F,G)=castl.R*/
               do{                                       /* y traverses ray, or:     */
                  H=y=h?Y^h:y+r;                           /* sneak in prev. best move */
                  if(y&M)break;                            /* board edge hit           */
                  m=E-S&b[E]&&y-E<2&E-y<2?I:m;             /* bad castling             */
                  if(p<3&y==E)H^=16;                       /* shift capt.sqr. H if e.p.*/
                  t=b[H];if(t&k|p<3&!(y-x&7)-!t)break;     /* capt. own, bad pawn mode */
                  i=37*w[t&7]+(t&192);                     /* value of capt. piece t   */
                  m=i<0?I:m;                               /* K capture                */
                  if(m>=l&d>1)goto C;                      /* abort on fail high       */
                  v=d-1?e:i-p;                             /* MVV/LVA scoring          */
                  if(d-!t>1)                               /* remaining depth          */
                  {
                     v=p<6?b[x+8]-b[y+8]:0;                  /* center positional pts.   */
                     b[G]=b[H]=b[x]=0;b[y]=u|32;             /* do move, set non-virgin  */
                     if(!(G&M))b[F]=k+6,v+=50;               /* castling: put R & score  */
                     v-=p-4|R>29?0:20;                       /* penalize mid-game K move */
                     if(p<3)                                 /* pawns:                   */
                     {v-=9*((x-2&M||b[x-2]-u)+               /* structure, undefended    */
                        (x+2&M||b[x+2]-u)-1              /*        squares plus bias */
                           +(b[x^16]==k+36))                 /* kling to non-virgin King */
                           -(R>>2);                          /* end-game Pawn-push bonus */
                                                               //user_promo()
                        V=y+r+1&S?647-p:2*(u&y+16&32);         /* PROMOTION or 6/7th bonus */
                        Wire.beginTransmission(boardSA);       // Transmit piece promoted value
                        Wire.write((int)V);               
                        Wire.endTransmission();
                        cpu_promo((int)V);                     // Computer Promotion call
                        b[y]+=V;i+=V;                          /* change piece, add score  */
                     }
                     v+=e+i;V=m>q?m:q;                       /* new eval and alpha       */
                     C=d-1-(d>5&p>2&!t&!h);
                     C=R>29|d<3|P-I?C:d;                     /* extend 1 ply if in check */
                     do
                        s=C>2|v>V?-D(-l,-V,-v,                 /* recursive eval. of reply */
                                       F,0,C):v;        /* or fail low if futile    */
                     W(s>q&++C<d);v=s;
                     if(z&&K-I&&v+I&&x==K&y==L)              /* move pending & in root:  */
                     {Q=-e-i;O=F;                            /*   exit if legal & found  */
                        R+=i>>7;--Z;return l;                  /* captured non-P material  */
                     }
                     b[G]=k+6;b[F]=b[y]=0;b[x]=u;b[H]=t;     /* undo move,G can be dummy */
                  }
                  if(v>m)                                  /* new best, update max,best*/
                     m=v,X=x,Y=y|S&F;                        /* mark double move with S  */
                  if(h){h=0;goto A;}                       /* redo after doing old best*/
                  if(x+r-y|u&32|                           /* not 1st step,moved before*/
                     p>2&(p-4|j-7||                        /* no P & no lateral K move,*/
                     b[G=x+3^r>>1&7]-k-6                   /* no virgin R in corner G, */
                     ||b[G^1]|b[G^2])                      /* no 2 empty sq. next to R */
                     )t+=p<5;                               /* fake capt. for nonsliding*/
                  else F=y;                                /* enable e.p.              */
               }
               W(!t);                                   /* if not capt. continue ray*/
            }
         }
      }
      W((x=x+9&~M)-B);                          /* next sqr. of board, wrap */
      C:if(m>I-M|m<M-I)d=98;                         /* mate holds to any depth  */
      m=m+I|P==I?m:0;                              /* best loses K: (stale)mate*/
      if(z&&d>2)
      {
         *c='a'+(X&7);c[1]='8'-(X>>4);c[2]='a'+(Y&7);c[3]='8'-(Y>>4&7);c[4]=0;
         breakpoint=2;           /* AVR Studio 4 Breakpoint for moves, watch c[] */
         char buff[150];
         #ifdef DEBUG
            sprintf(buff, "%2d ply, %9d searched, score=%6d by %c%c%c%c\n",d-1,N-S,m,
               'a'+(X&7),'8'-(X>>4),'a'+(Y&7),'8'-(Y>>4&7)); /* uncomment for Kibitz */
            Serial.print(buff);
         #endif
      }
   }                                             /*    encoded in X S,8 bits */
   k^=24;                                        /* change sides back        */
   --Z;return m+=m<e;                            /* delayed-loss bonus       */
}

void serialBoard(){
   Serial.println(" +-----------------+");
   for(int i=0; i<8; i++){
      Serial.print(8-i);
      Serial.print("| ");
      for(int j=0; j<8; j++){
         char c = sym[b[16*i+j]&15];
         Serial.print(c);
         Serial.print(' ');
      }
      Serial.println('|');
   }
   Serial.println(" +-----------------+");
   Serial.println("   a b c d e f g h");
  
   Serial.print("char b[]={");
   for(int i=0; i< 16*8; i++){
      Serial.print((int)b[i]);
      Serial.print(",");
   }
   Serial.println("0};");
  
   getByteBoard();
   char buff[11];
   for(int i=0; i<8; i++){
      sprintf(buff, "0x%08lX, ", byteBoard[i]);
      Serial.print(buff);
   }
   Serial.println('|');  
}

void getByteBoard(){
   uint32_t n1, n2;
   char sym[17] = {0x0,0xF,0x1,0x3,0x6,0x4,0x2,0x5,0xF,0x9,0xF,0xB,0xE,0xC,0xA,0xD};
                   
   for(int i=0; i<8; i++){
      byteBoard[i] = 0;
      for(int j=0; j<8; j+=2){
         n1 = sym[b[16*i+j+0]&15];
         n2 = sym[b[16*i+j+1]&15];
         byteBoard[i] |= (n1<<((7-j)*4)) | (n2 << ((6-j)*4));
      }
   }    
}

void searchDataBase(){    
   c[0]=0;             /* Flag to set move not found */
   getByteBoard();     /* Convert the board to a binary representation searchable in DB */
   int i, j;
   for(i=0; i < INDM; i++){                        /* Loop INDM amount blocks */
      boolean found=true;
      int block = ((seed+i)%INDM)*9;              /* Start in any block      */
      for(j=0; j<8; j++){
         uint32_t db = pgm_read_dword_near(dataBase+block+j);
         if( byteBoard[j] != db ){
            found=false;
            break;
         }
      }
        
      if(found){
         uint32_t db = pgm_read_dword_near(dataBase+block+j);
         c[4]=0;
         c[3] = (char)(db&0x000000FF)+'0';
         db>>=8;
         c[2] = (char)(db&0x000000FF)-0xA+'a';
         db>>=8;
         c[1] = (char)(db&0x000000FF)+'0';
         db>>=8;
         c[0] = (char)(db&0x000000FF)-0xA+'a';
         break;   
      }
   }   
}

void bkp(){
   for(int i=0; i<16*8+1; i++){
      bk[i] = b[i];
   }
}
void rst(){
   for(int i=0; i<16*8+1; i++){
      b[i] = bk[i];
   }
}

char board[10][17];

void displayBoard(int ind){ 
   // Put the pieces
   for(int y=0; y<2; y++){
      if(ind+y != 0 && ind+y != 9){
         for(int x=0; x<16; x+=2){
            putPiece(x+1, y, board[ind+y][x+1]);
         }
      }
   }  
}

void putPiece(int x, int y, char piece){
   int ind=0;
   switch(piece){
      case 'i':
      case 'p':
         ind=1;
         break;
        
      case 'P':
         ind=2;
         break;
        
      case 'r':
         ind=7;
         break;
        
      case 'R':
         ind=7;
         break;
        
      case 'k':
         ind=4;
         break;
        
      case 'K':
         ind=4;
         break;
        
      case 'b':
         ind=5;
         break;
        
      case 'B':
         ind=5;
         break;
        
      case 'q':
         ind=3;
         break;
        
      case 'Q':
         ind=3;
         break;
        
      case 'n':
         ind=6;
         break;
        
      case 'N':
         ind=6;
         break;
        
      case '.':
         ind = 0;
         break;
        
      case '-':
         return;
         break;
        
      default:;
   }
}

void UserBestMove() {
   int r;
   // Save the current state of the board
   bkp();
   // Calculate the best move for the user
   k ^= 24; // Switch sides for user move
   r = D(-I, I, Q, O, 0, 3); // Calculate human's best move
   k ^= 24; // Switch back sides for computer's move
   if (!(r > -I + 1)) {
      Serial.println("Error: Human's best move calculation failed.");
      return;
   }
   // Output the best move for the user
   Serial.print("Human's best move: ");
   Serial.println(c);
   strcpy(bestMove,c);
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
   moveReceived=true;
}

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
    
   // button dimensions (makes it easier to read/edit where they're drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 50
   #define BUTTON_W 100
   #define BUTTON_H 30
   #define BUTTON_SPACING_X 10
   #define BUTTON_SPACING_Y 10
   #define BUTTON_TEXTSIZE 2

   tft.fillScreen(BLACK);

   status(F("Select side:"));

   // Draw new buttons
   for (uint8_t row = 0; row < 2; row++) {
      for (uint8_t col = 0; col < 1; col++) {
         side_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
            BUTTON_Y+row*(BUTTON_H+BUTTON_SPACING_Y), // x, y, w, h, outline, fill, text
            BUTTON_W, BUTTON_H, ILI9341_WHITE, sidecolors[col + row], ILI9341_WHITE,
            sidelabels[col + row], BUTTON_TEXTSIZE);
         side_choice[col + row].drawButton();
      }
   }
    
   status_coord(10, 120, F("Select difficulty:"));

   for (uint8_t row = 0; row < 3; row++) {
      for (uint8_t col = 0; col < 1; col++) {
         diff_choice[col + row].initButton(&tft, BUTTON_X+col*(BUTTON_W+BUTTON_SPACING_X), 
            160+row*(BUTTON_H+BUTTON_SPACING_Y), BUTTON_W, BUTTON_H,
            ILI9341_WHITE,  diffcolors[col + row], ILI9341_WHITE,
            difflabels[col + row], BUTTON_TEXTSIZE);
         diff_choice[col + row].drawButton();
      }
   }

   confirm.initButton(&tft, BUTTON_X, 300, BUTTON_W, BUTTON_H,
               ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Done", BUTTON_TEXTSIZE); 
   confirm.drawButton();

   if(isWhiteSelected) {
      side_choice[0].drawButton(true);  // draw inverted version of button
      side_choice[1].drawButton(false);
   } else {
      side_choice[0].drawButton(false);  // draw inverted version of button
      side_choice[1].drawButton(true);
   }

   if (T==0x01) {
      diff_choice[0].drawButton(true);  // draw inverted version of button
      diff_choice[1].drawButton(false); // make sure other buttons revert to original color
      diff_choice[2].drawButton(false);
   } else if (T==0x20) {
      diff_choice[0].drawButton(false);  // draw inverted version of button
      diff_choice[1].drawButton(true); // make sure other buttons revert to original color
      diff_choice[2].drawButton(false);
   } else if (T==0x3F) {
      diff_choice[0].drawButton(false);  // draw inverted version of button
      diff_choice[1].drawButton(false);  // make sure other buttons revert to original color
      diff_choice[2].drawButton(true);
   } else {
      diff_choice[0].drawButton(false);  // draw inverted version of button
      diff_choice[1].drawButton(false);  // make sure other buttons revert to original color
      diff_choice[2].drawButton(false);
   }
     
   while(true) {
      // Listen for touchscreen input
      TSPoint p = ts.getPoint();
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);

      // Check if the touch input is detected
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         // Normalize the touch coordinates
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }
            
         if (side_choice[0].contains(p.x, p.y)) {    // white
            k=16;
            isWhiteSelected=true;
            tft.fillScreen(BLACK);
            tft.setRotation(2);

            side_choice[0].drawButton(true);  // draw inverted version of button
            side_choice[1].drawButton(false); // make sure other button reverts to original color
            setup_menu();
            return;
         } else if (side_choice[1].contains(p.x, p.y)) {     // black
            k^=24;
            isWhiteSelected=false;
            tft.fillScreen(BLACK);
            tft.setRotation(0);

            side_choice[1].drawButton(true);  // draw inverted version of button
            side_choice[0].drawButton(false); // make sure other button reverts to original color
            setup_menu();
            return;
         }

         if (diff_choice[0].contains(p.x, p.y)) {
            T = 0x01;  // easy
            diff_choice[0].drawButton(true);  // draw inverted version of button
            diff_choice[1].drawButton(false); // make sure other buttons revert to original color
            diff_choice[2].drawButton(false);
         } else if (diff_choice[1].contains(p.x, p.y)) {
            T = 0x20; // medium
            diff_choice[1].drawButton(true);  // draw inverted version of button
            diff_choice[0].drawButton(false); // make sure other buttons revert to original color
            diff_choice[2].drawButton(false);
         } else if (diff_choice[2].contains(p.x, p.y)) {
            T = 0x3F; // hard
            diff_choice[2].drawButton(true);  // draw inverted version of button
            diff_choice[0].drawButton(false); // make sure other buttons revert to original color
            diff_choice[1].drawButton(false);
         }

         if (confirm.contains(p.x, p.y)) {
            // Handle confirm button press
            confirm.drawButton(true); // Invert the button
            confirm.drawButton(false);
            return; // Exit the function to prevent further processing
         }
      }
   }
   Wire.beginTransmission(boardSA);
   Wire.write(k);          // Transmit playing side
   Wire.endTransmission();
}

void setup_board() {
   tft.fillScreen(BLACK);
  
   status(F("Please confirm the  board has been set  up according to the manual."));
  
   // button dimensions (makes it easier to read/edit where they're drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 50
   #define BUTTON_W 100
   #define BUTTON_H 30
   #define BUTTON_SPACING_X 10
   #define BUTTON_SPACING_Y 10
   #define BUTTON_TEXTSIZE 2

   confirm.initButton(&tft, BUTTON_X, 200, BUTTON_W, BUTTON_H,
                  ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Confirm", BUTTON_TEXTSIZE); 
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
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      

         if(confirm.contains(p.x, p.y)) {
            confirmed = true;
            confirm.drawButton(true);  // draw inverted version of buttons
         }
      }
   }
  tft.fillScreen(BLACK);
  // Start game here
}

void user_promo() {
   tft.fillScreen(BLACK);
   status(F("You're promoting a  pawn. Select which  piece you're        promoting to:"));

   // button dimensions (makes it easier to read/edit where they're being drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 100
   #define BUTTON_W 80
   #define BUTTON_H 30
   #define BUTTON_SPACING_X 20
   #define BUTTON_SPACING_Y 20
   #define BUTTON_TEXTSIZE 2

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
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      
      }

      //check if all buttons were pressed
      for (uint8_t b=0; b<4; b++) {
         if (promo_choice[b].contains(p.x, p.y)) {
            promo_done = true;
            promo_choice[b].drawButton(true);  // draw inverted version of button
            switch (b) {
               case 0: promo_to = 'q'; break;
               case 1: promo_to = 'r'; break;
               case 2: promo_to = 'k'; break;
               case 3: promo_to = 'b'; break;
               default: break;
            }
            delay(100); //time for user to see button invert
            tft.fillScreen(BLACK);
            switch (b) {
               case 0: status(F("Promoted to queen.")); break;
               case 1: status(F("Promoted to rook.")); break;
               case 2: status(F("Promoted to knight.")); break;
               case 3: status(F("Promoted to bishop.")); break;
               default: break;
            }
            // delay(5000); then call ingame menu function
         }
      }
   }
}

void cpu_promo(int cpu_promo_to) {
   // button dimensions (makes it easier to read/edit where they're being drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 160
   #define BUTTON_W 80
   #define BUTTON_H 30

   Serial.print("CPU promo value: ");
   Serial.println(cpu_promo_to);
   tft.fillScreen(BLACK);

   status_coord(10, 10, F("The computer is     promoting a pawn.   Please replace it   with an available "));
   if(cpu_promo_to==0)
      status_coord(10, 74, F("queen."));
   else if(cpu_promo_to==1)
      status_coord(10, 74, F("rook."));
   else if(cpu_promo_to==2)
      status_coord(10, 74, F("knight."));
   else if(cpu_promo_to==3)
      status_coord(10, 74, F("bishop."));

   done.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                 ILI9341_WHITE, ILI9341_DARKGREY, ILI9341_WHITE, "Done", 2); 
                     // x, y, w, h, outline, fill, text
   done.drawButton();

   while(!cpu_promo_done) {
      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      
      }

      if(done.contains(p.x, p.y)) {
         done.drawButton(true); // invert button colors
         cpu_promo_done = true;
         done.press(true);
         tft.fillScreen(BLACK); // for testing, can be removed later if creating unnecessary delay
      }
   }
}

void illegal_move_alert() {
   bool confirmed = false;

   tft.fillScreen(0xC800); // dark red
   status(F("You can't make that move. Please reset  the board to its    previous state and  then confirm."));

   // button dimensions (makes it easier to read/edit where they're drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 200
   #define BUTTON_W 100
   #define BUTTON_H 30

   confirm.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                  ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Confirm", 2); 
                  // x, y, w, h, outline, fill, text
   confirm.drawButton();

   while(!confirmed) {

      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }
      }

      if(confirm.contains(p.x, p.y)) {
         confirmed = true;
         confirm.drawButton(true); // invert button color
         tft.fillScreen(BLACK);
      }
   }
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
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }  

         if (yn[0].contains(p.x, p.y)) { // forfeit confirmed
            yn[0].drawButton(true); // invert button colors
            decided = true;
            tft.fillScreen(BLACK);
            // call game over screen
            game_over();
         }
         if (yn[1].contains(p.x, p.y)) { // forfeit denied
            yn[1].drawButton(true); // invert button colors
            decided = true;
            tft.fillScreen(BLACK);
            user_forfeit = false;
            loop();
         }    
      }
   }
}

void ingame_menu() {
   if(!user_forfeit) {

      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }   

         if(ingame[0].justReleased()) { // makes it only redraw if button was just released (will flash otherwise)
            if(hints_on) {
               ingame[0].drawButton(true);  // draw inverted version of button
            } else {
               ingame[0].drawButton(false); // draw regular button
            }
         } 

         if(ingame[1].justReleased()) {
            if(possible_moves_on) {
               ingame[1].drawButton(true);  // draw inverted version of button
               //print_possible_moves(1, 0, KNIGHT, WHITE, true, true);  //Knight at b1
            } else {
               ingame[1].drawButton(false); // draw regular button
            }
         }   

         if (ingame[0].contains(p.x, p.y)) {
            ingame[0].press(true);  // tell the button it is pressed
            ingame[0].drawButton(true);
            hints_on = !hints_on; // toggle hint variable
            user_hint(bestMove);
            ingame[0].drawButton(false);
         } else if (ingame[1].contains(p.x, p.y)) {
            ingame[1].press(true);  // tell the button it is pressed
            ingame[1].drawButton(true);
            possible_moves_on = !possible_moves_on; // toggle possible moves variable
            delay(1000);
            ingame[1].drawButton(false);
         } else if (ingame[2].contains(p.x, p.y)) {
            ingame[2].press(true);  // tell the button it is pressed
            ingame[2].drawButton(true);  // draw inverted version of button
            user_forfeit = true;
            delay(1000); //time for user to see button invert
            ingame[2].drawButton(false);
            tft.fillScreen(BLACK);
            forfeit_confirm();
         } else { // in testing, this seemed necessary for delay in this particular case, idk why
            ingame[0].press(false);
            ingame[1].press(false);
            ingame[2].press(false);
         } 
      } else { // in testing, this seemed necessary for delay in this particular case, idk why
         ingame[0].press(false);
         ingame[1].press(false);
         ingame[2].press(false);
      } 
   }
}

void you_win() {

  bool continued = false;

  status(F("You have the        computer in         checkmate. You win!"));

  // button dimensions (makes it easier to read/edit where they're drawn below)
  #define BUTTON_X 120
  #define BUTTON_Y 200
  #define BUTTON_W 100
  #define BUTTON_H 30

  cont.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                 ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Continue", 2); 
                     // x, y, w, h, outline, fill, text
  cont.drawButton();

   while(!continued) {

      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
     
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      
      }

      if(cont.contains(p.x, p.y)) {
         cont.drawButton(true);
         continued = true;
         tft.fillScreen(BLACK);
      }
   }
   game_over();
}

void you_lose() {

   bool continued = false;

   status(F("The computer has    you in checkmate.   You lose!"));

   // button dimensions (makes it easier to read/edit where they're drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 200
   #define BUTTON_W 100
   #define BUTTON_H 30

   cont.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                  ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Continue", 2); 
                  // x, y, w, h, outline, fill, text
   cont.drawButton();

   while(!continued) {

      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
     
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      
      }

      if(cont.contains(p.x, p.y)) {
         cont.drawButton(true);
         continued = true;
         tft.fillScreen(BLACK);
      }
   }
   game_over();
}

void game_over() {

   bool restart = false;

   status(F("Game over! Start    another?"));

   // button dimensions (makes it easier to read/edit where they're drawn below)
   #define BUTTON_X 120
   #define BUTTON_Y 200
   #define BUTTON_W 100
   #define BUTTON_H 30

   start.initButton(&tft, BUTTON_X, BUTTON_Y, BUTTON_W, BUTTON_H,
                  ILI9341_WHITE, ILI9341_BLACK, ILI9341_WHITE, "Start", 2); 
                  // x, y, w, h, outline, fill, text
   start.drawButton();

   while(!restart) {

      digitalWrite(13, HIGH);
      TSPoint p = ts.getPoint();
      digitalWrite(13, LOW);
  
      pinMode(XM, OUTPUT);
      pinMode(YP, OUTPUT);
  
      if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
         p.x = map(p.x, TS_MINX, TS_MAXX, tft.width(), 0);
         if (isWhiteSelected)
         {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
         } else {
            p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, 0, tft.height()));
         }      
      }

      if(start.contains(p.x, p.y)) {
         start.drawButton(true);
         restart = true;
         tft.fillScreen(BLACK);
         reboot();
      }
   }
}

void reboot() {
  wdt_disable();
  wdt_enable(WDTO_15MS);
  while (1) {}
}

void light_possible_move(char possible_move[2]) {
  square_conv_dst(possible_move[0], possible_move[1]);
  //test segment printing resulting binary to serial monitor
  Serial.print("Dst: ");
    Serial.print((int)d5dst); Serial.print((int)d4dst);
    Serial.print((int)d3dst); Serial.print((int)d2dst);
    Serial.print((int)d1dst); Serial.println((int)d0dst);
/*
  //loop to power pins based on binary digits
  while(hints_on) // while user's turn {
	if(d5dst) dp5 = HIGH; else dp5 = LOW;
	if(d4dst) dp4 = HIGH; else dp4 = LOW;
	if(d3dst) dp3 = HIGH; else dp3 = LOW;
	if(d2dst) dp2 = HIGH; else dp2 = LOW;
	if(d1dst) dp1 = HIGH; else dp1 = LOW;
	if(d0dst) dp0 = HIGH; else dp0 = LOW;
  }
*/
  delay(1000); //wait a second before showing another possible move
}

void user_hint(char best_move[5]) {
   //separate functions that do the same thing - could easily be condensed if
   //necessary by separating into one function to convert each digit, but this
   //would make the main code longer
   square_conv_src(best_move[0], best_move[1]);
   square_conv_dst(best_move[2], best_move[3]);

   //test loop printing resulting binary to serial monitor
   Serial.print("Testing "); Serial.println(best_move);
   Serial.print("Src: ");
   Serial.print((int)d5src); Serial.print((int)d4src);
   Serial.print((int)d3src); Serial.print((int)d2src);
   Serial.print((int)d1src); Serial.println((int)d0src);
   Serial.print("Dst: ");
   Serial.print((int)d5dst); Serial.print((int)d4dst);
   Serial.print((int)d3dst); Serial.print((int)d2dst);
   Serial.print((int)d1dst); Serial.println((int)d0dst);

/*   loop to power pins based on binary digits
   while(hints_on) // while user's turn {
      if(d5src) dp5 = HIGH; else dp5 = LOW;
      if(d4src) dp4 = HIGH; else dp4 = LOW;
      if(d3src) dp3 = HIGH; else dp3 = LOW;
      if(d2src) dp2 = HIGH; else dp2 = LOW;
      if(d1src) dp1 = HIGH; else dp1 = LOW;
      if(d0src) dp0 = HIGH; else dp0 = LOW;

      if(d5dst) dp5 = HIGH; else dp5 = LOW;
      if(d4dst) dp4 = HIGH; else dp4 = LOW;
      if(d3dst) dp3 = HIGH; else dp3 = LOW;
      if(d2dst) dp2 = HIGH; else dp2 = LOW;
      if(d1dst) dp1 = HIGH; else dp1 = LOW;
      if(d0dst) dp0 = HIGH; else dp0 = LOW;
   }
*/
   delay(1000);
}

//converts source square to binary (changes global variables)
void square_conv_src (char let, char num) {
  if(num == '5' | num == '6' | num == '7' | num == '8')
    d5src = false;
  else
    d5src = true;
  if(num == '3' | num == '4' | num == '7' | num == '8')
    d4src = false;
  else
    d4src = true;
  if(num == '2' | num == '4' | num == '6' | num == '8')
    d3src = false;
  else
    d3src = true;
  if(let == 'e' | let == 'f' | let == 'g' | let == 'h')
    d2src = true;
  else
    d2src = false;
  if(let == 'c' | let == 'd' | let == 'g' | let == 'h')
    d1src = true;
  else
    d1src = false;
  if(let == 'b' | let == 'd' | let == 'f' | let == 'h')
    d0src = true;
  else
    d0src = false;
}

//converts destination square to binary (changes global variables)
void square_conv_dst (char let, char num) {
  if(num == '5' | num == '6' | num == '7' | num == '8')
    d5dst = false;
  else
    d5dst = true;
  if(num == '3' | num == '4' | num == '7' | num == '8')
    d4dst = false;
  else
    d4dst = true;
  if(num == '2' | num == '4' | num == '6' | num == '8')
    d3dst = false;
  else
    d3dst = true;
  if(let == 'e' | let == 'f' | let == 'g' | let == 'h')
    d2dst = true;
  else
    d2dst = false;
  if(let == 'c' | let == 'd' | let == 'g' | let == 'h')
    d1dst = true;
  else
    d1dst = false;
  if(let == 'b' | let == 'd' | let == 'f' | let == 'h')
    d0dst = true;
  else
    d0dst = false;
}

// Function to initialize the chessboard
void initializeChessboard() {
   // Place white pieces
   chessboard[0][0] = {ROOK, WHITE};
   chessboard[0][1] = {KNIGHT, WHITE};
   chessboard[0][2] = {BISHOP, WHITE};
   chessboard[0][3] = {QUEEN, WHITE};
   chessboard[0][4] = {KING, WHITE};
   chessboard[0][5] = {BISHOP, WHITE};
   chessboard[0][6] = {KNIGHT, WHITE};
   chessboard[0][7] = {ROOK, WHITE};
   for (int i = 0; i < BOARD_SIZE; i++) {
      chessboard[1][i] = {PAWN, WHITE};
   }

   // Place black pieces
   chessboard[7][0] = {ROOK, BLACK};
   chessboard[7][1] = {KNIGHT, BLACK};
   chessboard[7][2] = {BISHOP, BLACK};
   chessboard[7][3] = {QUEEN, BLACK};
   chessboard[7][4] = {KING, BLACK};
   chessboard[7][5] = {BISHOP, BLACK};
   chessboard[7][6] = {KNIGHT, BLACK};
   chessboard[7][7] = {ROOK, BLACK};
   for (int i = 0; i < BOARD_SIZE; i++) {
      chessboard[6][i] = {PAWN, BLACK};
   }

   // Initialize empty squares
   for (int i = 2; i < 6; i++) {
      for (int j = 0; j < BOARD_SIZE; j++) {
         chessboard[i][j] = {EMPTY, -1};
      }
   }
}

// Function to check if a square is within the board
bool withinBoard(int x, int y) {
   return (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE);
}

// Function to check if there is a piece at the given square
bool isPieceAtSquare(int fileIndex, int rankIndex) {
   return chessboard[rankIndex][fileIndex].type != EMPTY;
}

// Function to add a move to the list of possible moves
void addMove(String* possibleMoves, int* numMoves, int x, int y) {
   possibleMoves[*numMoves] = String((char)('a' + x)) + String(y + 1);
   (*numMoves)++;
}

// Function to get possible moves for a piece at a given square
void get_possible_moves(int fileIndex, int rankIndex, int pieceType, int pieceColor, bool canCastleKingSide, bool canCastleQueenSide, String* possibleMoves, int* numMoves) {
   // Pawn moves
   if (pieceType == PAWN) {
      // Pawns can move one square forward
      int direction = (pieceColor == WHITE) ? 1 : -1; // Define direction based on piece color
      if (withinBoard(fileIndex, rankIndex + direction) && !isPieceAtSquare(fileIndex, rankIndex + direction)) {
         addMove(possibleMoves, numMoves, fileIndex, rankIndex + direction);
      }
      // Pawns can optionally move two squares forward from the starting position
      if ((pieceColor == WHITE && rankIndex == 1) || (pieceColor == BLACK && rankIndex == 6)) {
         if (withinBoard(fileIndex, rankIndex + 2 * direction) && !isPieceAtSquare(fileIndex, rankIndex + direction) && !isPieceAtSquare(fileIndex, rankIndex + 2 * direction)) {
               addMove(possibleMoves, numMoves, fileIndex, rankIndex + 2 * direction);
         }
      }
      // Pawns can capture diagonally
      if (withinBoard(fileIndex - 1, rankIndex + direction) && isPieceAtSquare(fileIndex - 1, rankIndex + direction) && chessboard[rankIndex + direction][fileIndex - 1].color != pieceColor) {
         addMove(possibleMoves, numMoves, fileIndex - 1, rankIndex + direction);
      }
      if (withinBoard(fileIndex + 1, rankIndex + direction) && isPieceAtSquare(fileIndex + 1, rankIndex + direction) && chessboard[rankIndex + direction][fileIndex + 1].color != pieceColor) {
         addMove(possibleMoves, numMoves, fileIndex + 1, rankIndex + direction);
      }
   }
   // Knight moves
   else if (pieceType == KNIGHT) {
      int knightMoves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}, {1, -2}, {1, 2}, {2, -1}, {2, 1}};
      for (int i = 0; i < 8; i++) {
         int newX = fileIndex + knightMoves[i][0];
         int newY = rankIndex + knightMoves[i][1];
         if (withinBoard(newX, newY) && (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor)) {
            addMove(possibleMoves, numMoves, newX, newY);
         }
      }
   }
   // Bishop moves
   else if (pieceType == BISHOP || pieceType == QUEEN) {
      int bishopDirections[4][2] = {{1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
      for (int i = 0; i < 4; i++) {
         for (int j = 1; j < BOARD_SIZE; j++) {
            int newX = fileIndex + bishopDirections[i][0] * j;
            int newY = rankIndex + bishopDirections[i][1] * j;
            if (withinBoard(newX, newY)) {
               if (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor) {
                  addMove(possibleMoves, numMoves, newX, newY);
               } else {
                  if (pieceType != QUEEN) break; // Queen can continue beyond obstacles
                  else break; // If it is QUEEN we allow it to move to the captured position
               }
            } else {
               break;
            }
         }
      }
   }
   // Rook moves
   else if (pieceType == ROOK || pieceType == QUEEN) {
      int rookDirections[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};
      for (int i = 0; i < 4; i++) {
         for (int j = 1; j < BOARD_SIZE; j++) {
            int newX = fileIndex + rookDirections[i][0] * j;
            int newY = rankIndex + rookDirections[i][1] * j;
            if (withinBoard(newX, newY)) {
               if (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor) {
                  addMove(possibleMoves, numMoves, newX, newY);
               } else {
                  if (pieceType != QUEEN) break; // Queen can continue beyond obstacles
                     else break; // If it is QUEEN we allow it to move to the captured position
               }
            } else {
               break;
            }
         }
      }
   }
   // King moves
   else if (pieceType == KING) {
      int kingMoves[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1}, {0, 1}, {1, -1}, {1, 0}, {1, 1}};
      for (int i = 0; i < 8; i++) {
         int newX = fileIndex + kingMoves[i][0];
         int newY = rankIndex + kingMoves[i][1];
         if (withinBoard(newX, newY) && (!isPieceAtSquare(newX, newY) || chessboard[newY][newX].color != pieceColor)) {
            addMove(possibleMoves, numMoves, newX, newY);
         }
      }
      // Castling moves...
   }
}

// Function to print possible moves for a piece at a given square
void print_possible_moves(int fileIndex, int rankIndex, int pieceType, int pieceColor, bool canCastleKingSide, bool canCastleQueenSide) {
   String possibleMoves[BOARD_SIZE * BOARD_SIZE]; // Maximum possible moves
   int numMoves = 0;
   get_possible_moves(fileIndex, rankIndex, pieceType, pieceColor, canCastleKingSide, canCastleQueenSide, possibleMoves, &numMoves);

   Serial.print("Possible Moves for ");
   switch (pieceType) {
      case PAWN:
         Serial.print("Pawn");
         break;
      case KNIGHT:
         Serial.print("Knight");
         break;
      case BISHOP:
         Serial.print("Bishop");
         break;
      case ROOK:
         Serial.print("Rook");
         break;
      case QUEEN:
         Serial.print("Queen");
         break;
      case KING:
         Serial.print("King");
         break;
      default:
         Serial.print("Unknown Piece");
   }
   Serial.print(" at square ");
   Serial.print(char('a' + fileIndex));
   Serial.println(char('1' + rankIndex));

   for (int i = 0; i < numMoves; i++) {
      Serial.println("Move: " + possibleMoves[i]);
   }
}