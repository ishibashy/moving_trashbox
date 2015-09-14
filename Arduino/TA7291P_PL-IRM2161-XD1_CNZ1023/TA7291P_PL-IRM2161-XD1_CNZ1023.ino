#include <IRremote.h>
// モータ
//右
const int motorR1 = 8;
const int motorR2 = 9;
const int PWM_motR = 10;
//左
const int motorL1 = 11;
const int motorL2 = 12;
const int PWM_motL = 13;
//スピード
int speedL = 255;//後で直進する比を実測、255:237とか
int speedR = 255;
//IRモジュール
const int IRMDL = 7;
IRrecv irrecv(IRMDL);
decode_results results;
//フォトインタラプタ、change回数
int changeL = 0;
int changeR = 0;
//ブザー
const int buzzer = 6;
int melo = 500;
//測距モジュール
const int dstns_msrng = 0;
//関数のID
int id;

//-----初期化-----
void setup() {
  // モータ
  //右
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  //左
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  //IRモジュール
  irrecv.enableIRIn();
  //フォトインタラプタ
  attachInterrupt(1, photo_changingL, CHANGE);
  attachInterrupt(0, photo_changingR, CHANGE);
  //測距モジュール
  pinMode(dstns_msrng, OUTPUT);
  //ブザー
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
}
//----------------


//-----メインで使う関数-----

void photo_changingL() {                    //フォトインタラプタ左
  changeL++;
  Serial.print(changeL);
  Serial.println("times_changed_L");
  if (changeL > changeR) {
    speedL--;
    speedR = 255;//後で直進する比を実測
  }
}

void photo_changingR() {                    //フォトインタラプタ右
  changeR++;
  Serial.print(changeR);
  Serial.println("times_changed_R");
  if (changeR > changeL) {
    speedR--;
    speedL = 255;//後で直進する比を実測
  }
}

void cw(int melo){
  int melo_speed = 1;
  digitalWrite(buzzer,HIGH);
  delay(melo * melo_speed);
  digitalWrite(buzzer,LOW);
  delay(100 * melo_speed);
}

void CBuzzer(bool b) {                       //ブザー
  int count = 0;
  while (count < 8) {
    //tone(buzzer, 131, melo); //ド
    digitalWrite(buzzer,HIGH);
    delay(melo);
    digitalWrite(buzzer,LOW);
    delay(melo);
    count++;
  }
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

void barricade_check() {                    //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;
  Serial.print(distance);
  Serial.println("cm");
  //80cm以内を複数回検知してからにするかも
  if (distance < 80) {
    Serial.println("!!!STOP!!!");
    int tmpL = changeL;
    int tmpR = changeR;
    brake();
    CBuzzer(true);//ブザー1
    while (1)
      if ((6762 / (analogRead(0) - 9)) - 4 > 80)
        break;
    Serial.println("!!!RESTART!!!");    
    CBuzzer(false);//ブザー2
    changeL = tmpL;
    changeR = tmpR;
    if (id == 0)
      ahead();
    else
      ahead_d();
  }
}

void motor_speed() {                        //モータ速度呼び出し
  analogWrite(PWM_motR, speedL);
  analogWrite(PWM_motL, speedR);
}

void circle(int angle) {                    //回転
  if (angle == 180)
    while (1) {
      motor_speed();
      if (changeL > 19 || changeR > 19)
        break;
    }
  else if (angle == 135)
    while (1) {
      motor_speed();
      if (changeL > 14 || changeR > 14)
        break;
    }
  else
    while (1) {
      motor_speed();
      if (changeL > 9 || changeR > 9)
        break;
    }
  changeL = changeR = 0;
}

void ahead() {                              //前進
  id = 0;
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  while (1) {
    motor_speed();
    barricade_check();
    if (changeL > 682 || changeR > 682)
      break;
  }
  changeL = changeR = 0;
}


void ahead_d() {                            //前進斜め
  id = 1;
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  while (1) {
    motor_speed();
    barricade_check();
    if (changeL > 964 || changeR > 964)
      break;
  }
  changeL = changeR = 0;
}

void astern(int changechange) {             //後進
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedR);
  while (1) {
    motor_speed();
    if (changeL > changechange || changeR > changechange)
      break;
  }
  changeL = changeR = 0;
}

void brake() {                              //ブレーキ
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  int changechange = changeL;
  if (changechange < changeR)
    changechange = changeR;
  changeL = changeR = 0;
  astern(changechange);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
}

void left(int left) {                       //左旋回
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  circle(left);
}
void right(int right) {                     //右旋回
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedR);
  circle(right);
}
//--------------------------
//-----メイン-----
int id1 = 1;
int id2 = 2;
int angle1 = 90;
int angle2 = 135;
int angle3 = 180;
void loop() {

  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value, DEC);
    Serial.println(results.value, BIN);

    String receiveValue = "";
    receiveValue = (String)results.value;
    //Received_P :　これから向かう点
    //id1          :　今いる点
    //id2          :　前にいた点
    if (receiveValue == "3873901013") {
      Serial.println("Received_P1");
      if (id1 == 2) {
        if (id2 == 1)
          left(angle3);
        if (id2 == 3)
          left(angle1);
        if (id2 == 4)
          left(angle2);
        ahead();
        id2 = 2;
      } else if (id1 == 3) {
        if (id2 == 1)
          left(angle3);
        if (id2 == 2)
          right(angle2);
        if (id2 == 4)
          left(angle2);
        ahead_d();
        id2 = 3;
      } else if (id1 == 4) {
        if (id2 == 1)
          left(angle3);
        if (id2 == 2)
          right(angle2);
        if (id2 == 3)
          right(angle1);
        ahead();
        id2 = 4;
      }
      id1 = 1;
    } else if (receiveValue == "1513342804") {
      Serial.println("Received_P2");
      if (id1 == 1) {
        if (id2 == 2)
          left(angle3);
        Serial.println("angle180");
        if (id2 == 3)
          right(angle2);
        if (id2 == 4)
          right(angle1);
        ahead();
        Serial.println("ahead");
        id2 = 2;
      } else if (id1 == 3) {
        if (id2 == 1)
          left(angle2);
        if (id2 == 2)
          left(angle3);
        if (id2 == 4)
          left(angle1);
        ahead_d();
        id2 = 3;
      } else if (id1 == 4) {
        if (id2 == 1)
          left(angle2);
        if (id2 == 2)
          left(angle3);
        if (id2 == 3)
          right(angle2);
        ahead();
        id2 = 4;
      }
      id1 = 2;
    } else if (receiveValue == "4092259158") {
      Serial.println("Received_P3");
      if (id1 == 1) {
        if (id2 == 2)
          left(angle2);
        if (id2 == 3)
          left(angle3);
        if (id2 == 4)
          right(angle2);
        ahead_d();
        id2 = 1;
      } else if (id1 == 2) {
        if (id2 == 1)
          right(angle1);
        if (id2 == 3)
          left(angle3);
        if (id2 == 4)
          right(angle2);
        ahead();
        id2 = 2;
      } else if (id1== 4) {
        if (id2 == 1)
          left(angle1);
        if (id2 == 2)
          left(angle2);
        if (id2 == 3)
          left(angle3);
        ahead();
        id2 = 4;
      }
      id1= 3;
    } else if (receiveValue == "869376052") {
      Serial.println("Received_P4");
      if (id1 == 1) {
        if (id2 == 2)
          left(angle1);  //or  right(90);
        if (id2 == 3)
          left(angle2);  //or  right(45);
        if (id2 == 4)
          left(angle3); //or no_rotate
        ahead();  //or  astern();
        id2 = 1;
      } else if (id1 == 2) {
        if (id2 == 1)
          right(angle2);         //or left(45);
        if (id2 == 3)
          left(angle2);          //or right(45);
        if (id2 == 4)
          left(angle3);          //or no_rotate
        ahead_d();               //or  astern_();
        id2 = 2;
      } else if (id1 == 3) {
        if (id2 == 1)
          right(angle2);         //or left(45);
        if (id2 == 2)
          left(angle1);          //or right(90);
        if (id2 == 4)
          left(angle3);          //or no_rotate
        ahead();                 //or astern();
        id2 = 3;
      }
      id1 = 4;
    }
    brake();
    Serial.print("ima P");
    Serial.print(id2);
    Serial.println("ni irun.");
    Serial.print("P");
    Serial.print(id1);
    Serial.println("kara kitan.");
    Serial.println("----------");

    irrecv.resume();
  }
  delay(1000);
  Serial.print("speedL:");
  Serial.print(speedL);
  Serial.print(" speedR:");
  Serial.println(speedR);
}
//---------------

