
//-----初期化-----
void setup() {

 
  Serial.begin(9600);
}
//----------------


//-メインで使う関数-


void barricade_check() {                    //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;
  Serial.print(distance);
  Serial.println("cm");
  Serial.println(analogRead(0));
  //80cm以内を複数回検知してからにするかも
  if (distance < 50) {
    Serial.println("!!!STOP!!!");
    while (1){
      distance = (6762 / (analogRead(0) - 9)) - 4;
      Serial.print(distance);
      Serial.println("cm");
      Serial.println(analogRead(0));
      if ((6762 / (analogRead(0) - 9)) - 4 > 80)
        break;
      delay(50);
    }
    Serial.println("!!!RESTART!!!");    
  }
}

//-----メイン-----

void loop() {
  barricade_check();
  delay(100);
}
//---------------
