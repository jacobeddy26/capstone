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

#define  queen 24
#define knight 8
#define rook 16
#define bishop 12

#define scaraSA 3                                // Slave Address for SCARA controller
#define boardSA 2                                // Slave Address for Board/LCD controller
#define engineSA 1                               // Slave Address for Chess Engine controller

LiquidCrystal lcd(8, 13, 9, 4, 5, 6, 7);        // Pins to control LCD display
int adc_key_val[5] ={50, 200, 400, 600, 800 };  // Analog values from keypad

char bestMove[5]; // Initialize best move globally
bool moveReceived = false;
bool isWhiteSelected = true;
char inputMove[5];
char outputMove[6];
int moveCounter = 0;
bool isCastle = false;
bool isCapture = false;
bool hints_on=false;
bool userMove = false;
int count = 0;

//global variables that hold binary digits for source and destination squares
bool d5src, d4src, d3src, d2src, d1src, d0src;
bool d5dst, d4dst, d3dst, d2dst, d1dst, d0dst;

#define dp0 33
#define dp1 31
#define dp2 29
#define dp3 27
#define dp4 25
#define dp5 23
#define ENABLE 37

void setup(){
    Wire.begin(engineSA);
    //Wire.onReceive(receiveEvent);
    Serial.begin(115200);
    Serial.println("  *** CHESSuino ***\n");
    
    lcd.clear(); 
    lcd.begin(16, 2);  
    
    lastH[0] = 0;
    characters();
    countWhitePieces();
    pinMode(13, OUTPUT);
    pinMode(dp5,OUTPUT);
    pinMode(dp4,OUTPUT);
    pinMode(dp3,OUTPUT);
    pinMode(dp2,OUTPUT);
    pinMode(dp1,OUTPUT);
    pinMode(dp0,OUTPUT);
    pinMode(ENABLE,OUTPUT);

    digitalWrite(dp5,LOW);
    digitalWrite(dp4,LOW);
    digitalWrite(dp3,LOW);
    digitalWrite(dp2,LOW);
    digitalWrite(dp1,LOW);
    digitalWrite(dp0,LOW);
    digitalWrite(ENABLE,LOW);
}

void loop(){
   //serialBoard();
    int r;
    int tempCount = count;
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
    T=0x01;                                   /* T=Computer Play strength */
    bkp();                                    /* Save the board just in case */    
    r = D(-I,I,Q,O,1,3);                      /* Check & do the human movement */
    if( !(r>-I+1) ){
        lcd.setCursor(10, 1);
        lcd.print("Lose "); 
        gameOver();
    } else if (r!=8000 && moveCounter>1) {
      Serial.println("Illegal Move!");
    } else {

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
        T=0x01;                               /* T=Computer Play strength */
        r = D(-I,I,Q,O,1,3);                  /* Check & do*/
        if( !(r>-I+1) ) gameOver();
        if(k == 0x08){
            lcd.setCursor(10, 1);            
            lcd.print("ERR DB");
            gameOver();
        }

       countWhitePieces();
       Serial.print("count: "); Serial.print(count); Serial.print(" tempCount: "); Serial.println(tempCount);
       if(tempCount>count) {
         isCapture=true;
       }
        
       outputMove[0]='?';
       outputMove[1]=c[0];
       outputMove[2]=c[1];
       outputMove[3]=c[2];
       outputMove[4]=c[3];
       // Throw a flag if the calculated move is a capture or castle
       if (isCapture) {                             // Set flag for capture
          outputMove[0]='x';                      
       } else if (isCastle) {                       // Set flag for castle
          outputMove[0]='o';               
       }
       Wire.beginTransmission(scaraSA);          // Transmit normal move
       Wire.write(outputMove); 
       Wire.endTransmission();
       Serial.print("Output Move: "); 
       Serial.print(outputMove); Serial.println("\n");
       isCapture = false;
       isCastle = false;
        
       strcpy(lastM, c);                         /* Valid ARDUINO movement */
       return;
      }
    
    K=I;
    N=0;
    T=0x01;                                   /* T=Computer Play strength */
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

    countWhitePieces();
    Serial.print("count: "); Serial.print(count); Serial.print(" tempCount: "); Serial.println(tempCount);
    if(tempCount>count) {
       isCapture=true;
    }

    outputMove[0]='?';
    outputMove[1]=c[0];
    outputMove[2]=c[1];
    outputMove[3]=c[2];
    outputMove[4]=c[3];
    // Throw a flag if the calculated move is a capture or castle
    if (isCapture) {                             // Set flag for capture
       outputMove[0]='x';
    } else if (isCastle) {                       // Set flag for castle                   
       outputMove[0]='o';
    }
    Wire.beginTransmission(scaraSA);          // Transmit normal move
    Wire.write(outputMove); 
    Wire.endTransmission();
    Serial.print("Output Move: "); 
    Serial.print(outputMove); Serial.println("\n");
    isCapture = false;
    isCastle = false;
    
    strcpy(lastM, c);                         /* Valid ARDUINO movement */

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
    if (mn==1 && strcmp(c,"a1a1")==0)
    {
      return;
    }
    int sss=0;
    
    lcd.setCursor(0,1);
    lcd.print("                ");
    printMN(mn, 1);

    printMove();
    for(;;) {
      int key = waitForKey();
      delay(200);
      switch(key) {
            case 0:   // RIGHT
               if(sss==0 && x1 <7){ x1++;}
               if(sss==1 && x2 <7){ x2++;}
               break;
            case 1:   // UP
               if(sss==0 && y1 <7){ y1++;}
               if(sss==1 && y2 <7){ y2++;}
               break;
            case 2:   // DOWN
                if(sss==0 && y1 >0){ y1--;}
                if(sss==1 && y2 >0){ y2--;}
               break;
            case 3:   // LEFT
                if(sss==0 && x1 >0){ x1--;}
                if(sss==1 && x2 >0){ x2--;}
               break;
            case 4:   // SELECT  think
               if(sss==1) {
                    if(x2>=0 && y2>=0) {
                        c[0] = x1+'a';
                        c[1] = y1+'1';
                        c[2] = x2+'a';
                        c[3] = y2+'1';
                        c[4] = 0;
                        // No tone to validate movement
                        return;
                    }else{
                        // Move invalid
                    }                        
               }            
               if(sss==0){
                  if(x1>=0 && y1>=0){
                     sss=1;
                  }else{
                     if(hints_on) {
                        digitalWrite(ENABLE,LOW);
                        hints_on=false;
                     } else if( !hints_on) {
                        hints_on=true;
                        user_hint();
                     }
                  }
               } 
               break;
      }
      if (moveReceived) {
         strcpy(c,inputMove);
         Serial.println(c);
         moveReceived=false;
         delay(100);
         break;
      }
      printMove();
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
 isCapture=false;
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
   u=b[x];                                     /* scan board looking for   */
   if(u&k){                                    /*  own piece (inefficient!)*/
    r=p=u&7;                                   /* p = piece type (set r>0) */
    j=o[p+16];                                 /* first step vector f.piece*/
    W(r=p>2&r<0?-r:-o[++j])                    /* loop over directions o[] */
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
      {v=p<6?b[x+8]-b[y+8]:0;                  /* center positional pts.   */
       b[G]=b[H]=b[x]=0;b[y]=u|32;             /* do move, set non-virgin  */
       if(!(G&M))b[F]=k+6,v+=50,isCastle=true; /* castling: put R & score  */
       v-=p-4|R>29?0:20;                       /* penalize mid-game K move */
       if(p<3)                                 /* pawns:                   */
       {v-=9*((x-2&M||b[x-2]-u)+               /* structure, undefended    */
              (x+2&M||b[x+2]-u)-1              /*        squares plus bias */
             +(b[x^16]==k+36))                 /* kling to non-virgin King */
             -(R>>2);                          /* end-game Pawn-push bonus */
        V=y+r+1&S?647-p:2*(u&y+16&32);         /* PROMOTION or 6/7th bonus */
        Wire.beginTransmission(boardSA);       // Transmit piece promoted
        Wire.write((int)V);               
        Wire.endTransmission();
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
        R+=i>>7;--Z; return l;                  /* captured non-P material  */
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
     }W(!t);                                   /* if not capt. continue ray*/
  }}}W((x=x+9&~M)-B);                          /* next sqr. of board, wrap */
C:if(m>I-M|m<M-I)d=98;                         /* mate holds to any depth  */
  m=m+I|P==I?m:0;                              /* best loses K: (stale)mate*/
  if(z&&d>2)
   {*c='a'+(X&7);c[1]='8'-(X>>4);c[2]='a'+(Y&7);c[3]='8'-(Y>>4&7);c[4]=0;
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
    for(;;);
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
   userMove=true;
   int r;
   // Save the current state of the board
   bkp();
   // Calculate the best move for the user
   k ^= 24; // Switch sides for user move
   r = D(-I, I, Q, O, 0, 3); // Calculate human's best move
   k ^= 24; // Switch back sides for computer's move
   delay(200);
   if (moveCounter==0) {
      strcpy(c,"a1a1");
      moveCounter++;
      return;
   } else if (moveCounter==1) {
      moveCounter++;
   }
   strcpy(bestMove,c);
   delay(200);
   // Output the best move for the user
   //Serial.print("Human's best move: ");
   //Serial.println(bestMove);
   userMove=false;
   isCastle=false;
   isCapture=false;
}

/*
// function that executes whenever data is received from the master
// this function is registered as an event,  see setup()
void receiveEvent() {
   int i = 0;
   while (Wire.available() && i < 5) {
      inputMove[i] = Wire.read(); // Read char data
      i++;
   }
   inputMove[4] = 0; // Null-terminate the received char array
   moveReceived=true;
   delay(100);
}

void light_possible_move(char possible_move[2]) {
  square_conv_dst(possible_move[0], possible_move[1]);
  //test segment printing resulting binary to serial monitor
  Serial.print("Dst: ");
  Serial.print((int)d5dst); Serial.print((int)d4dst);
  Serial.print((int)d3dst); Serial.print((int)d2dst);
  Serial.print((int)d1dst); Serial.println((int)d0dst);
  
  //loop to power pins based on binary digits
  while(hints_on) // while user's turn {
	if(d5dst) dp5 = HIGH; else dp5 = LOW;
	if(d4dst) dp4 = HIGH; else dp4 = LOW;
	if(d3dst) dp3 = HIGH; else dp3 = LOW;
	if(d2dst) dp2 = HIGH; else dp2 = LOW;
	if(d1dst) dp1 = HIGH; else dp1 = LOW;
	if(d0dst) dp0 = HIGH; else dp0 = LOW;
  }

  delay(1000); //wait a second before showing another possible move
} */

void user_hint() {   // Calculate and output human's best move
   //separate functions that do the same thing - could easily be condensed if
   //necessary by separating into one function to convert each digit, but this
   //would make the main code longer
   Serial.print("Human's best move: "); Serial.println(bestMove);
   square_conv_src(bestMove[0], bestMove[1]);
   square_conv_dst(bestMove[2], bestMove[3]);
   /*test loop printing resulting binary to serial monitor
   Serial.print("Testing "); Serial.println(best_move);
   Serial.print("Src: ");
   Serial.print((int)d5src); Serial.print((int)d4src);
   Serial.print((int)d3src); Serial.print((int)d2src);
   Serial.print((int)d1src); Serial.println((int)d0src);
   Serial.print("Dst: ");
   Serial.print((int)d5dst); Serial.print((int)d4dst);
   Serial.print((int)d3dst); Serial.print((int)d2dst);
   Serial.print((int)d1dst); Serial.println((int)d0dst); */

  //loop to power pins based on binary digits
   if(hints_on) {
      digitalWrite(ENABLE,HIGH);

      if(d5src) digitalWrite(dp5, HIGH); else digitalWrite(dp5, LOW);
      if(d4src) digitalWrite(dp4, HIGH); else digitalWrite(dp4, LOW);
      if(d3src) digitalWrite(dp3, HIGH); else digitalWrite(dp3, LOW);
      if(d2src) digitalWrite(dp2, HIGH); else digitalWrite(dp2, LOW);
      if(d1src) digitalWrite(dp1, HIGH); else digitalWrite(dp1, LOW);
      if(d0src) digitalWrite(dp0, HIGH); else digitalWrite(dp0, LOW);

      delay(2000);

      if(d5dst) digitalWrite(dp5, HIGH); else digitalWrite(dp5, LOW);
      if(d4dst) digitalWrite(dp4, HIGH); else digitalWrite(dp4, LOW);
      if(d3dst) digitalWrite(dp3, HIGH); else digitalWrite(dp3, LOW);
      if(d2dst) digitalWrite(dp2, HIGH); else digitalWrite(dp2, LOW);
      if(d1dst) digitalWrite(dp1, HIGH); else digitalWrite(dp1, LOW);
      if(d0dst) digitalWrite(dp0, HIGH); else digitalWrite(dp0, LOW);
   } else {
      digitalWrite(ENABLE,LOW);
   }
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

void countWhitePieces() {
    count = 0;
    for (int i = 0; i < 129; i++) {
        if ((b[i] & 0x08) && 
               ((i>=0 && i<8) || (i>=16 && i<24) || (i>=32 && i<40) || (i>=48 && i<56) || 
                  (i>=64 && i<72) || (i>=80 && i<88) || (i>=96 && i<104) || (i>=112 && i<120))) { // Check if the index is valid and the piece is white
            count++;
        }
    }
}
