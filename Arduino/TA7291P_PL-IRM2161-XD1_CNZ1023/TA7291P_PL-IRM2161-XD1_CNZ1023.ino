#include <IRremote.h>

//#include <Servo.h>
//Servo ore_no_servo;
// モーター制御
//右
const int motorR1 = 8;//const:不変
const int motorR2 = 9;
const int PWM_motR = 10;
//左
const int motorL1 = 11;
const int motorL2 = 12;
const int PWM_motL = 13;
//IRモジュール
const int IRMDL = 7;
IRrecv irrecv(IRMDL);
decode_results results;
//フォトインタラプタ
const int INTRRPT = 2;
const int attchINTRRPT = 0;
//測距モジュール
const int dstns_msrng = 1;

//-----初期化-----
void setup() {
  // モーター制御
  pinMode(motorR1, OUTPUT); //信号用ピン
  pinMode(motorR2, OUTPUT); //信号用ピン
  pinMode(motorL1, OUTPUT); //信号用ピン
  pinMode(motorL2, OUTPUT); //信号用ピン
  //IRモジュール
  irrecv.enableIRIn(); // Start the receiver
  //フォトインタラプタ
  attachInterrupt(attchINTRRPT, photo_falling, CHANGE);
  Serial.begin(9600);
}
//----------------


//-----メインで使う関数-----

int rise = 0;
int valval = digitalRead(INTRRPT);
int change = 0;
//int fall = 0;
//void photo_falling() {         //フォトインタラプタ
//  int val = digitalRead(INTRRPT);
//  if (val == HIGH){
//    Serial.println("rising");
//  }else{
//    Serial.println("falling");
//  }
//}

void photo_falling() {         //フォトインタラプタ
  int val = digitalRead(INTRRPT);
  //if (valval != val){
   // valval = val;
    change++;
    Serial.print(change);
    Serial.println("times_changed");
 // }
}


void ahead() {               //前進
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, 255);
  delay(2000);
  while(1){
    if(rise>6*10)
    break;
  }
  
  
}
void ahead_d() {             //前進斜め
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, 255);
  delay(2000 * sqrt(2));
}

void astern() {      //後進
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, 255);
  //delay(time);
}

void brake() {  //ブレーキ
  rise = 0;
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  delay(300);
  int riserise = rise;
  rise = 0;
  astern();
  while(rise<riserise){ 
  }
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  rise = 0;
}

void left(int left) {        //左旋回
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, 255);
  delay(10 * left);
}
void right(int right) {     //右旋回
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, 255);
  delay(10 * right);
}
//--------------------------

int i = 1;
int j = 2;
int angle1 = 90;
int angle2 = 135;
int angle3 = 180;
//-----メイン-----
void loop() {

  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value, DEC);
    Serial.println(results.value, BIN);

    String s = "";
    s = (String)results.value;
    //Received_P :　これから向かう点
    //i          :　今いる点
    //j          :　前にいた点
    if (s == "3873901013") {
      Serial.println("Received_P1");
      if (i == 2) {
        if (j == 1)
          left(angle3);
        if (j == 3)
          left(angle1);
        if (j == 4)
          left(angle2);
        ahead();
        j = 2;
      } else if (i == 3) {
        if (j == 1)
          left(angle3);
        if (j == 2)
          right(angle2);
        if (j == 4)
          left(angle2);
        ahead_d();
        j = 3;
      } else if (i == 4) {
        if (j == 1)
          left(angle3);
        if (j == 2)
          right(angle2);
        if (j == 3)
          right(angle1);
        ahead();
        j = 4;
      }
      i = 1;
    } else if (s == "1513342804") {
      Serial.println("Received_P2");
      if (i == 1) {
        if (j == 2)
          left(100);
        Serial.println("angle180");
        if (j == 3)
          right(angle2);
        if (j == 4)
          right(angle1);
        ahead();
        Serial.println("ahead");
        j = 2;
      } else if (i == 3) {
        if (j == 1)
          left(angle2);
        if (j == 2)
          left(angle3);
        if (j == 4)
          left(angle1);
        ahead_d();
        j = 3;
      } else if (i == 4) {
        if (j == 1)
          left(angle2);
        if (j == 2)
          left(angle3);
        if (j == 3)
          right(angle2);
        ahead();
        j = 4;
      }
      i = 2;
    } else if (s == "4092259158") {
      Serial.println("Received_P3");
      if (i == 1) {
        if (j == 2)
          left(angle2);
        if (j == 3)
          left(angle3);
        if (j == 4)
          right(angle2);
        ahead_d();
        j = 1;
      } else if (i == 2) {
        if (j == 1)
          right(angle1);
        if (j == 3)
          left(angle3);
        if (j == 4)
          right(angle2);
        ahead();
        j = 2;
      } else if (i == 4) {
        if (j == 1)
          left(angle1);
        if (j == 2)
          left(angle2);
        if (j == 3)
          left(angle3);
        ahead();
        j = 4;
      }
      i = 3;
    } else if (s == "869376052") {
      Serial.println("Received_P4");
      if (i == 1) {
        if (j == 2)
          left(angle1);  //or  right(90);
        if (j == 3)
          left(angle2);  //or  right(45);
        if (j == 4)
          left(angle3); //or no_rotate
        ahead();  //or  astern();
        j = 1;
      } else if (i == 2) {
        if (j == 1)
          right(angle2);         //or left(45);
        if (j == 3)
          left(angle2);          //or right(45);
        if (j == 4)
          left(angle3);          //or no_rotate
        ahead_d(); //or  astern_();
        j = 2;
      } else if (i == 3) {
        if (j == 1)
          right(angle2); //or left(45);
        if (j == 2)
          left(angle1);   //or right(90);
        if (j == 4)
          left(angle3);  //or no_rotate
        ahead();   //or astern();
        j = 3;
      }
      i = 4;
    }
    //astern(0);
    brake();
    Serial.print("ima P");
    Serial.print(j);
    Serial.println("ni irun.");
    Serial.print("P");
    Serial.print(i);
    Serial.println("kara kitan.");
    Serial.println("----------");
    
    irrecv.resume(); // Receive the next value
  }
delay(1000);
Serial.println(valval);
}
//---------------

