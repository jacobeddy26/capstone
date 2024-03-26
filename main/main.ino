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
char difflabels[3][9] = {"   Easy  ", "  Medium ", "   Hard  "};
uint16_t diffcolors[3] = {ILI9341_DARKGREEN, ILI9341_ORANGE, ILI9341_RED};

//creates in-game options buttons for user
Elegoo_GFX_Button ingame[2];
char ingame_labels[2][12] = {"Hints", "Forfeit"};
uint16_t ingame_colors[2] = {ILI9341_BLACK, ILI9341_BLACK};
bool hints_on = false; // must be global as other screens may be called in the meantime
bool user_forfeit = false;

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

#define pawn 1
#define knight 2
#define bishop 3
#define rook 4
#define queen 5
#define king 6

#define scaraSA 3                                // Slave Address for SCARA controller
#define boardSA 2                                // Slave Address for Board/LCD controller
#define engineSA 1                               // Slave Address for Chess Engine controller

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);        // Pins to control LCD display
int adc_key_val[5] ={50, 200, 400, 600, 800 };  // Analog values from keypad

char bestMove[5] = {0}; // Initialize best move globally
char inputMove[5];
char outputMove[6];

bool isCastle, isCapture;

void setup(){
   Wire.begin(engineSA);
   Wire.onReceive(receiveEvent);
   Serial.begin(115200);
   Serial.println("  *** CHESSuino ***");
    
   lcd.clear(); 
   lcd.begin(16, 2);  

   tft.reset();
   uint16_t identifier = tft.readID();
   if(identifier != 0x9325 & identifier != 0x9328 & identifier != 0x4535 & identifier != 0x7575 & identifier != 0x8357)
      identifier = 0x9341;
   tft.begin(identifier);
   tft.setRotation(2);
   tft.fillScreen(BLACK);
 
   setup_menu();

   lastH[0] = 0;
   characters();
   pinMode(13, OUTPUT);
}

void loop(){
   ingame_menu();
   int r;
   digitalWrite(13, LOW);
  
   // Print last movements
   printLastMovs();

   // Calculate and output human's best move
   UserBestMove();

   // Take move from human
   x1=x2=y1=y2=-1;
   takeMove();
   lcd.setCursor(10, 1);
   lcd.print("Think");                       /* Turn for ARDUINO */
    
   K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;      /* parse entered move */
   N=0;
   T=0x3F;                                   /* T=Computer Play strength */
   bkp();                                    /* Save the board just in case */    
   r = D(-I,I,Q,O,1,3);                      /* Check & do the human movement */
   if( !(r>-I+1) ){
      lcd.setCursor(10, 1);
      lcd.print("Lose "); 
      gameOver();
   } else {
      //Serial.println("Move not found!");
      illegal_move_alert();
   }
   if(k == 0x10){                            /* The flag turn must change to 0x08 */
      lcd.setCursor(10, 1);
      lcd.print("     ");
      return;
   }
  
   // Input human move
   strcpy(lastH, c);                         /* Valid human movement */

   mn++;                                     /* Next move */
   searchDataBase();                         /* Search in database */
   if(c[0]){                                 /* Movement found */
      lcd.setCursor(10, 1);
      lcd.print(c);
      lcd.print(" *");
      K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;  /* parse move found */
      N=0;
      T=0x3F;                               /* T=Computer Play strength */
      r = D(-I,I,Q,O,1,3);                  /* Check & do*/
      if( !(r>-I+1) ) gameOver();
      if(k == 0x08){
         lcd.setCursor(10, 1);            
         lcd.print("ERR DB");
         gameOver();
      }
        
      strcpy(lastM, c);                         /* Valid ARDUINO movement */

      outputMove[0]='#';
      outputMove[1]=c[1];
      outputMove[2]=c[2];
      outputMove[3]=c[3];
      outputMove[4]=c[4];
      // Throw a flag if the calculated move is a capture or castle
      if (isCapture) {
         outputMove[0]='x';
         Wire.beginTransmission(scaraSA);          // beginTransmission(address)
         Wire.write(outputMove,6); 
         Wire.endTransmission();                   // Output Computer's Move as String to I2C    } else if(isCastle) {           
         // Set flag for castle
      } else if (isCastle) {
         outputMove[0]='o';
         Wire.beginTransmission(scaraSA);          // beginTransmission(address)
         Wire.write(outputMove,6); 
         Wire.endTransmission();                   // Output Computer's Move as String to I2C
      }

      delay(1000);        
      return;
   }
    
   K=I;
   N=0;
   T=0x3F;                                   /* T=Computer Play strength */
   r = D(-I,I,Q,O,1,3);                      /* Think & do computer's move*/    

   if( !(r>-I+1) ){
      lcd.setCursor(10, 1);
      lcd.print("Lose*"); 
      gameOver();
   }
    
   if(k == 0x08){                            /* Some times the algorithm do not */
      lcd.setCursor(10, 1);                 /* execute the move and do not change */
      lcd.print("ERR 3 ");                  /* the turn flag */
      gameOver();                           /* 1. b1c3  c7c5?       2. f2f4? */
   }

   strcpy(lastM, c);                         /* Valid ARDUINO movement */
   
   outputMove[0]='#';
   outputMove[1]=c[1];
   outputMove[2]=c[2];
   outputMove[3]=c[3];
   outputMove[4]=c[4];
   // Throw a flag if the calculated move is a capture or castle
   if (isCapture) {
      outputMove[0]='x';
      Wire.beginTransmission(scaraSA);          // beginTransmission(address)
      Wire.write(outputMove,6); 
      Wire.endTransmission();                   // Output Computer's Move as String to I2C    } else if(isCastle) {           
      // Set flag for castle
   } else if (isCastle) {
      outputMove[0]='o';
      Wire.beginTransmission(scaraSA);          // beginTransmission(address)
      Wire.write(outputMove,6); 
      Wire.endTransmission();                   // Output Computer's Move as String to I2C
   }

   strcpy(c, "a1a1");                        /* Execute a invalid move to check score again */
   r = D(-I,I,Q,O,1,3);
   if( !(r>-I+1) ){
      lcd.setCursor(10, 1);
      lcd.print(lastM);
      lcd.print(" ");
      gameOver();
   }
   if(k == 0x08){                            /* Some times the algorithm do not */
      lcd.setCursor(10, 1);                 /* execute the move and do not change */
      lcd.print("ERR 3 ");                  /* the turn flag */
      gameOver();                           /* 1. b1c3  c7c5?       2. f2f4? */
   }    
   delay(500);
}

void takeMove(){  
   lcd.setCursor(0,1);
   lcd.print("                ");
   printMN(mn, 1);

   printMove();
   for(;;){
      if (inputMove[0] != 'n')
      {
         strcpy(c,inputMove);
         printMove();
         break;
      }
   }
}

void printMove(){
   lcd.setCursor(4, 1);
   if(x1>=0) lcd.print((char)(x1+'a')); else lcd.print('_');
   if(y1>=0) lcd.print((char)(y1+'1')); else lcd.print('_');
   if(x2>=0) lcd.print((char)(x2+'a')); else lcd.print('_');
   if(y2>=0) lcd.print((char)(y2+'1')); else lcd.print('_');
}

void printMN(int n, int y){
   if(n<=9){
      lcd.setCursor(1, y);
   }else{
      lcd.setCursor(0, y);
   }
   lcd.print(n);
   lcd.print('.');
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

// Convert ADC value to key number
int getKey(){
   int k;
   unsigned int input = analogRead(KEYBOARD_INPUT);
   
   for (k = 0; k < NUM_KEYS; k++){
      if (input < adc_key_val[k]){
         return k;
      }
   }
   
   if (k >= NUM_KEYS)k = -1;  // No valid key pressed
   return k;
}

int waitForKey(){
   int res;
   do{
      seed++;
      res = getKey();
   }while(res<0);
   return res;
}

void gameOver(){
   //for(;;);
   you_win();
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

void boardNavigate(){
   strcpy(board[0], " a b c d e f g h");
   strcpy(board[9], board[0]);
    
   for(int i=0; i<8; i++){
      strcpy(board[i+1], "                ");
      for(int j=0; j<8; j++){
         board[i+1][0]=(char)(8-i+'0');
         char c = sym[b[16*i+j]&15];
         board[i+1][j*2+1] = c;
      }
   }    

   int ind=0;
   displayBoard(ind);
   for(;;){
      int k = waitForKey();
      delay(200);
        
      switch(k){
         case 1:   // UP
            if(ind>0){
               ind--;
               displayBoard(ind);
            }
            break;
            
         case 2:   // DOWN
            if(ind<8){
               ind++;
               displayBoard(ind);
            }                
            break;
            
         case 4:   // SELECT
            // After navigating to a square, display possible moves for the piece on that square
            return;
            break;
      }
   }
}

void displayBoard(int ind){
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print(board[ind+0]);
   lcd.setCursor(0, 1);
   lcd.print(board[ind+1]);
    
   // Put the pieces
   for(int y=0; y<2; y++){
      if(ind+y != 0 && ind+y != 9){
         for(int x=0; x<16; x+=2){
            putPiece(x+1, y, board[ind+y][x+1]);
         }
      }
   }  
}

void printLastMovs(){
   lcd.clear();    
   if(mn > 1){
      printMN(mn-1, 0);
      lcd.setCursor(4, 0);
      lcd.print(lastH);
      lcd.setCursor(10, 0);
      lcd.print(lastM);
   }else{
      lcd.setCursor(0,0);
      lcd.print("** CHESSuino **");
   }
}

/**
  0=square
  1=pawnB
  2=pawnW
  3=queen  B&W  
  4=king   B&W
  5=bishop B&W
  6=knight B&W
  7=rock   B&W
*/
void characters(){
   lcd.createChar(0, esq);
   lcd.createChar(1, pawnB);
   lcd.createChar(2, pawnW);
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
         lcd.createChar(7, rockB);
         ind=7;
         break;
        
      case 'R':
         lcd.createChar(7, rockW);
         ind=7;
         break;
        
      case 'k':
         lcd.createChar(4, kingB);
         ind=4;
         break;
        
      case 'K':
         lcd.createChar(4, kingW);
         ind=4;
         break;
        
      case 'b':
         lcd.createChar(5, bishopB);
         ind=5;
         break;
        
      case 'B':
         lcd.createChar(5, bishopW);
         ind=5;
         break;
        
      case 'q':
         lcd.createChar(3, queenB);
         ind=3;
         break;
        
      case 'Q':
         lcd.createChar(3, queenW);
         ind=3;
         break;
        
      case 'n':
         lcd.createChar(6, knightB);
         ind=6;
         break;
        
      case 'N':
         lcd.createChar(6, knightW);
         ind=6;
         break;
        
      case '.':
         ind = 0;
         break;
        
      case '-':
         return;
         break;
        
      default:
         lcd.print('x');
   }
   lcd.setCursor(x, y);
   lcd.write(ind);
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
         p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
      }

      //check if all buttons were pressed
      for (uint8_t b=0; b<4; b++) {
         if (promo_choice[b].contains(p.x, p.y)) {
            promo_done = true;
            promo_choice[b].drawButton(true);  // draw inverted version of button
            switch (b) {
               case 0: promo_to = "q"; break;
               case 1: promo_to = "r"; break;
               case 2: promo_to = "k"; break;
               case 3: promo_to = "b"; break;
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
         p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
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

  tft.fillScreen(0xC800); // dark red
  status(F("You can't make that move. Please reset the board to its    previous state and  then confirm."));

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
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     if(confirm.contains(p.x, p.y)) {
         confirmed = true;
         confirm.drawButton(true); // invert button color
         tft.fillScreen(BLACK);
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
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
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
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
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
       p.y = (tft.height()-map(p.y, TS_MINY, TS_MAXY, tft.height(), 0));
     }

     if(start.contains(p.x, p.y)) {
        start.drawButton(true);
        restart = true;
        tft.fillScreen(BLACK);
     }
  }
}