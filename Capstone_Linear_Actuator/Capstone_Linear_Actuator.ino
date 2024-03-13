int switch1=8;
int switch2=9;
int magnet=10;
int input;
#define MAX_THROW 2000

void setup() {
  // put your setup code here, to run once:
  pinMode(switch1,OUTPUT);
  pinMode(switch2,OUTPUT);
  pinMode(magnet,OUTPUT);
  Serial.begin(115200);
}

void loop() {
  extend();
  hold(8000);
  retract();
  //digitalWrite(magnet,HIGH);
}

void retract()
{
   Serial.println("Retracting!");
   digitalWrite(switch1,LOW);
   digitalWrite(switch2,HIGH);
   delay(MAX_THROW);
}

void hold(int time)
{
   Serial.println("Holding!");
   digitalWrite(switch1,LOW);
   digitalWrite(switch2,LOW);
   delay(time+MAX_THROW);
}

void extend()
{
   Serial.println("Extending!");
   digitalWrite(switch1,HIGH);
   digitalWrite(switch2,LOW);
   delay(MAX_THROW);
}

