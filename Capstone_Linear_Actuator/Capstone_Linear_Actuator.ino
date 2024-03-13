int switch1=8;
int switch2=9;
int magnet=10;
int input;

void setup() {
  // put your setup code here, to run once:
  pinMode(switch1,OUTPUT);
  pinMode(switch2,OUTPUT);
  pinMode(magnet,OUTPUT);
  Serial.begin(115200);
}

void loop() {
 
 /*
  extend();
  delay(3000);
  hold();
  delay(3000);
  */
  digitalWrite(magnet,HIGH);
  /*
  extend();
  delay(3000);
  hold();
  delay(3000);
  retract();
  delay(3000);
  */
  

  
}

void retract()
{
  digitalWrite(switch1,LOW);
  digitalWrite(switch2,HIGH);
}

void hold()
{
  digitalWrite(switch1,HIGH);
  digitalWrite(switch2,HIGH);
}

void extend()
{
  digitalWrite(switch1,HIGH);
  digitalWrite(switch2,LOW);
}

