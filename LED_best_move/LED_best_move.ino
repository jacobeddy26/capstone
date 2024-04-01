//global variables that hold binary digits for source and destination squares
bool d5src, d4src, d3src, d2src, d1src, d0src;
bool d5dst, d4dst, d3dst, d2dst, d1dst, d0dst;

void setup() {
  Serial.begin(9600);
  //hardcoded square ID for testing only
  //(this and the function call
  // together perform the function of the chess engine)
/*  char best_move[4] = {'g','7','h','7'};
  user_hint(best_move);
*/
  //hardcoded dst squares for testing only
  char possible_move_1[2] = {'a','2'};
  char possible_move_2[2] = {'a','4'};
  char possible_move_3[2] = {'b','3'};
  light_possible_move(possible_move_1);
  light_possible_move(possible_move_2);
  light_possible_move(possible_move_3);
  
  delay(1000);
}

void light_possible_move(char possible_move[2]) {
  square_conv_dst(possible_move[0], possible_move[1]);
  //test segment printing resulting binary to serial monitor
  Serial.print("Dst: ");
    Serial.print((int)d5dst); Serial.print((int)d4dst);
    Serial.print((int)d3dst); Serial.print((int)d2dst);
    Serial.print((int)d1dst); Serial.print((int)d0dst);
    Serial.print('\n');
/*
  //loop to power pins based on binary digits
  while(1) // while user's turn {
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

void user_hint(char best_move[4]) {
  //separate functions that do the same thing - could easily be condensed if
  //necessary by separating into one function to convert each digit, but this
  //would make the main code longer
  square_conv_src(best_move[0], best_move[1]);
  square_conv_dst(best_move[2], best_move[3]);

  //test loop printing resulting binary to serial monitor
  while(1) {
    Serial.print("Testing "); Serial.print(best_move);
    Serial.print('\n');
    Serial.print("Src: ");
    Serial.print((int)d5src); Serial.print((int)d4src);
    Serial.print((int)d3src); Serial.print((int)d2src);
    Serial.print((int)d1src); Serial.print((int)d0src);
    Serial.print('\n');
    Serial.print("Dst: ");
    Serial.print((int)d5dst); Serial.print((int)d4dst);
    Serial.print((int)d3dst); Serial.print((int)d2dst);
    Serial.print((int)d1dst); Serial.print((int)d0dst);
    Serial.print('\n');
    delay(1000);
  }
/*
  //loop to power pins based on binary digits
  while(1) // while user's turn {
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
}

//converts source square to binary (changes global variables)
void square_conv_src (char let, char num) {
  if(num == '5' | num == '6' | num == '7' | num == '8')
    d5src = true;
  else
    d5src = false;
  if(num == '3' | num == '4' | num == '7' | num == '8')
    d4src = true;
  else
    d4src = false;
  if(num == '2' | num == '4' | num == '6' | num == '8')
    d3src = true;
  else
    d3src = false;
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
    d5dst = true;
  else
    d5dst = false;
  if(num == '3' | num == '4' | num == '7' | num == '8')
    d4dst = true;
  else
    d4dst = false;
  if(num == '2' | num == '4' | num == '6' | num == '8')
    d3dst = true;
  else
    d3dst = false;
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

void loop() {}
