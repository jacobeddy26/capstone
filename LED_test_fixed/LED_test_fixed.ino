void setup() {
  // init ports (6 for inputs, 7th for OE0)
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
}

void loop() {
  //000000
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  delay(500);
  //000001
  digitalWrite(2, HIGH);
  delay(500);
  //000010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //000011
  digitalWrite(2, HIGH);
  delay(500);
  //000100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //000101
  digitalWrite(2, HIGH);
  delay(500);
  //000110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //000111
  digitalWrite(2, HIGH);
  delay(500);
  //001000
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //001001
	//ENABLE LOW
	//digitalWrite(10, LOW);
  digitalWrite(2, HIGH);
  delay(500);
  //001010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //001011
  digitalWrite(2, HIGH);
  delay(500);
  //001100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //001101
	//ENABLE HIGH
	//digitalWrite(10, HIGH);
  digitalWrite(2, HIGH);
  delay(500);
  //001110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //001111
  digitalWrite(2, HIGH);
  delay(500);
  //010000
  digitalWrite(6, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //010001
  digitalWrite(2, HIGH);
  delay(500);
  //010010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //010011
  digitalWrite(2, HIGH);
  delay(500);
  //010100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //010101
  digitalWrite(2, HIGH);
  delay(500);
  //010110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //010111
  digitalWrite(2, HIGH);
  delay(500);
  //011000
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //011001
  digitalWrite(2, HIGH);
  delay(500);
  //011010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //011011
  digitalWrite(2, HIGH);
  delay(500);
  //011100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //011101
  digitalWrite(2, HIGH);
  delay(500);
  //011110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //011111
	//ENABLE LOW
	//digitalWrite(10, LOW);
  digitalWrite(2, HIGH);
  delay(500);
  //100000
  digitalWrite(7, HIGH);
  digitalWrite(6, LOW);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //100001
  digitalWrite(2, HIGH);
  delay(500);
  //100010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //100011
  digitalWrite(2, HIGH);
  delay(500);
  //100100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //100101
  digitalWrite(2, HIGH);
  delay(500);
  //100110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //100111
  digitalWrite(2, HIGH);
  delay(500);
  //101000
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //101001
	//ENABLE HIGH
	//digitalWrite(10, HIGH);
  digitalWrite(2, HIGH);
  delay(500);
  //101010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //101011
  digitalWrite(2, HIGH);
  delay(500);
  //101100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //101101
  digitalWrite(2, HIGH);
  delay(500);
  //101110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //101111
  digitalWrite(2, HIGH);
  delay(500);
  //110000
  digitalWrite(6, HIGH);
  digitalWrite(5, LOW);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //110001
  digitalWrite(2, HIGH);
  delay(500);
  //110010
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //110011
  digitalWrite(2, HIGH);
  delay(500);
  //110100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //110101
  digitalWrite(2, HIGH);
  delay(500);
  //110110
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //110111
  digitalWrite(2, HIGH);
  delay(500);
  //111000
  digitalWrite(5, HIGH);
  digitalWrite(4, LOW);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //111001
  digitalWrite(2, HIGH);
  delay(500);
  //111010
	//ENABLE LOW
	//digitalWrite(10, LOW);
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //111011
  digitalWrite(2, HIGH);
  delay(500);
  //111100
  digitalWrite(4, HIGH);
  digitalWrite(3, LOW);
  digitalWrite(2, LOW);
  delay(500);
  //111101
  digitalWrite(2, HIGH);
  delay(500);
  //111110
	//ENABLE HIGH
	//digitalWrite(10, HIGH);
  digitalWrite(3, HIGH);
  digitalWrite(2, LOW);
  delay(500);
  //111111
  digitalWrite(2, HIGH);
  delay(500);
}