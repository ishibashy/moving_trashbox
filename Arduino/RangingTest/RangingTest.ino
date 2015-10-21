const int DMMDL = 5;
const int analogPin = 3;
int high = 255;
//-----初期化-----
void setup() {
  pinMode(DMMDL, OUTPUT);
  analogWrite(DMMDL,255);
  
  pinMode(analogPin, OUTPUT);
  analogWrite(analogPin, HIGH);
  Serial.begin(9600);
}
//----------------


//-メインで使う関数-


void barricade_check() {                    //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;
  Serial.println((String)distance+"cm:"+(String)analogRead(0));

}

//-----メイン-----

void loop() {
  barricade_check();
  delay(100);
}
//---------------
