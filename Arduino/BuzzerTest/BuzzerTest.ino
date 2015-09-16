//ブザー
const int buzzer = 6;
int melo = 500;


//-----初期化-----
void setup() {

  //ブザー
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);

   CBuzzer(false);
}
//----------------


//-メインで使う関数-

void cw(double melo){
  double melo_speed = 0.5;
  digitalWrite(buzzer,HIGH);
  delay(melo * melo_speed);
  digitalWrite(buzzer,LOW);
  delay(100 * melo_speed);
}

void CBuzzer(bool b) {                       //ブザー
  int count = 0;
//  while (count < 8) {
//    //tone(buzzer, 131, melo); //ド
//    digitalWrite(buzzer,HIGH);
//    delay(melo);
//    digitalWrite(buzzer,LOW);
//    delay(melo);
//    count++;
//  }
  if(b){
    cw(100);cw(100);cw(100);//ブザー1
    delay(200);
    cw(300);cw(300);cw(300);
    delay(200);
    cw(100);cw(100);cw(100);
    b = false;
  }else{
    cw(300);cw(300);cw(100);//ブザー2
    delay(200);
    cw(300);cw(300);cw(300);
    b = true;
  }
}


void loop() {


  delay(1000);


}
//---------------
