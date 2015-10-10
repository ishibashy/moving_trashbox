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
int volatile changeL = 0;
int volatile changeR = 0;
//測距モジュール
int count = 0;//pin番号ではない
//ブザー
const int buzzer = 6;
int melo = 500;
//関数のID
int method_id;

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
  //ブザー
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
}
//----------------

//--割り込み関数--
void photo_changingL() {                    //フォトインタラプタ左
  changeL++;
  Serial.print(changeL);
  Serial.println("times_changed_L");
//  if (changeL > changeR) {
//    speedL--;
//    speedR = 255;//後で直進する比を実測
//  }
}

void photo_changingR() {                    //フォトインタラプタ右
  changeR++;
  Serial.print(changeR);
  Serial.println("times_changed_R");
//  if (changeR > changeL) {
//    speedR--;
//    speedL = 255;//後で直進する比を実測
//  }
}
//----------------
//-メインで使う関数-

void cw(int melo){                            //トンツートンツー・ー・ー
  int melo_speed = 1;
  digitalWrite(buzzer,HIGH);
  delay(melo * melo_speed);
  digitalWrite(buzzer,LOW);
  delay(100 * melo_speed);
}

void CBuzzer(bool b) {                       //ブザー
  Serial.print("\t\t\tenterd CBuzzer"+(String)b+")");
//  int count = 0;
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
//    delay(200);
//    cw(300);cw(300);cw(300);
//    delay(200);
//    cw(100);cw(100);cw(100);
    b = false;
  }else{
    cw(300);cw(300);cw(100);//ブザー2
//    delay(200);
//    cw(300);cw(300);cw(300);
    b = true;
  }
  Serial.println("\t\t\texit CBuzzer"+(String)b+")");
}

void barricade_check() {                    //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;
  Serial.println("\t\t"+(String)distance+"cm");

  if (distance < 80) 
    count++;
  else
    count = 0;
      
  if(count > 20) {
    Serial.println("\t\t!!!STOP!!!");
    int tmpL = changeL;
    int tmpR = changeR;
    brake();
    Serial.println("\t\tenter CBuzzer(true)");
    CBuzzer(true);//ブザー1
    Serial.println("\t\texited CBuzzer(true)");
    while (1){
      distance = (6762 / (analogRead(0) - 9)) - 4;
      Serial.println("\t\t\t"+(String)distance+"cm");
      if ((6762 / (distance - 9)) - 4 > 80)
        count--;
      if(count < 1)
        break;
    }
    Serial.println("\t\t!!!RESTART!!!");
    Serial.println("\t\tenter CBuzzer(false)");    
    CBuzzer(false);//ブザー2
    Serial.println("\t\texited CBuzzer(false)");
    changeL = tmpL;
    changeR = tmpR;
    if (method_id == 0)
      ahead();
    else
      ahead_d();
  }
}

void motor_speed() {                        //モータ速度更新
  analogWrite(PWM_motR, speedL);
  analogWrite(PWM_motL, speedR);
}

void circle(int angle) {                    //回転
  Serial.println("\t\t\tentered circle("+(String)angle+")");
  if (angle == 180)
//    while (1) {
//      motor_speed();
//      if (changeL > 19 || changeR > 19)
//          break;
//    }
    while (changeL < 12 || changeR < 12) {
      motor_speed();
    }
  else if (angle == 135)
    while (changeL < 14 || changeR < 14) {
      motor_speed();
    }
  else
    while (changeL < 9 || changeR < 9) {
      motor_speed();
    }
  changeL = changeR = 0;
  Serial.println("\t\t\texit circle("+(String)angle+")");
}

void ahead() {                              //前進
  Serial.println("\t\tentered ahead()");
  method_id = 0;
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  Serial.println("\t\tenter while(1)");
  while (1) {
    motor_speed();
    barricade_check();
    if (changeL > 682 || changeR > 682)
      break;
  }
  Serial.println("\t\texited while(1)");
  changeL = changeR = 0;
  Serial.println("\t\texit ahead())");
}


void ahead_d() {                            //前進斜め
  Serial.println("\t\tentered ahead_d()");
  method_id = 1;
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  Serial.println("\t\tenter while(1)");
  while (1) {
    motor_speed();
    barricade_check();
    if (changeL > 964 || changeR > 964)
      break;
  }
  Serial.println("\t\texited while(1)");
  changeL = changeR = 0;
  Serial.println("\t\texit ahead_d())");
}

void astern(int changechange) {             //後進
  Serial.println("\t\tentered astern("+(String)changechange+")");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedR);
  Serial.println("\t\tenter while(1)");
  while (1) {
    motor_speed();
    if (changeL >= changechange || changeR >= changechange){
      Serial.println("\t\t\texited while(1)");
      break;
    }
  }
  Serial.println("\t\texited while(1)");
  changeL = changeR = 0;
  Serial.println("\t\texit astern("+(String)changechange+")");
}

void brake() {                              //ブレーキ
  Serial.println("\tentered brake()");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  int changechange = changeL;
  if (changechange < changeR)
    changechange = changeR;
  changeL = changeR = 0;
  Serial.println("\tenter astern("+(String)changechange+")");
  astern(changechange);
  Serial.println("\texited astern("+(String)changechange+")");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  Serial.println("\texit brake()");
}

void left(int left) {                       //左旋回
  Serial.println("\t\tentered left("+(String)left+")");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  Serial.println("\t\tenter circle("+(String)left+")");
  circle(left);
  Serial.println("\t\texited circle("+(String)left+")");
  Serial.println("\t\texit left("+(String)left+")");
}
void right(int right) {                     //右旋回
  Serial.println("\t\tentered right("+(String)right+")");
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedR);
  Serial.println("\t\tenter circle("+(String)right+")");
  circle(right);
  Serial.println("\t\texited circle("+(String)right+")");
  Serial.println("\t\texit right("+(String)right+")");
}

void receiveIR(String receiveValue,int *id){    //受信
Serial.println("\tentered receiveValue()");
const int angle1 = 90;
const int angle2 = 135;
const int angle3 = 180;
 if (receiveValue == "3887053538") {
      Serial.println("\tReceived_P1");
      if (id[0] == 2) {
        if (id[1] == 1)
          left(angle3);
        if (id[1] == 3)
          left(angle1);
        if (id[1] == 4)
          left(angle2);
        ahead();
        id[1] = 2;
      } else if (id[0] == 3) {
        if (id[1] == 1)
          left(angle3);
        if (id[1] == 2)
          right(angle2);
        if (id[1] == 4)
          left(angle2);
        ahead_d();
        id[1] = 3;
      } else if (id[0] == 4) {
        if (id[1] == 1)
          left(angle3);
        if (id[1] == 2)
          right(angle2);
        if (id[1] == 3)
          right(angle1);
        ahead();
        id[1] = 4;
      }
      id[0] = 1;
    } else if (receiveValue == "1513342804") {
      Serial.println("\tReceived_P2");
      if (id[0] == 1) {
        if (id[1] == 2){
          Serial.println("\tenter left("+(String)angle3+")");
          left(angle3);
          Serial.println("\texited left("+(String)angle3+")");
        }
        if (id[1] == 3)
          right(angle2);
        if (id[1] == 4)
          right(angle1);
        Serial.println("\tenter ahead()");  
        ahead();
        Serial.println("\texied ahead()");
        id[1] = 1;
      } else if (id[0] == 3) {
        if (id[1] == 1)
          left(angle2);
        if (id[1] == 2)
          left(angle3);
        if (id[1] == 4)
          left(angle1);
        ahead_d();
        id[1] = 3;
      } else if (id[0] == 4) {
        if (id[1] == 1)
          left(angle2);
        if (id[1] == 2)
          left(angle3);
        if (id[1] == 3)
          right(angle2);
        ahead();
        id[1] = 4;
      }
      id[0] = 2;
    } else if (receiveValue == "4092259158") {
      Serial.println("\tReceived_P3");
      if (id[0] == 1) {
        if (id[1] == 2)
          left(angle2);
        if (id[1] == 3)
          left(angle3);
        if (id[1] == 4)
          right(angle2);
        ahead_d();
        id[1] = 1;
      } else if (id[0] == 2) {
        if (id[1] == 1)
          right(angle1);
        if (id[1] == 3)
          left(angle3);
        if (id[1] == 4)
          right(angle2);
        ahead();
        id[1] = 2;
      } else if (id[0]== 4) {
        if (id[1] == 1)
          left(angle1);
        if (id[1] == 2)
          left(angle2);
        if (id[1] == 3)
          left(angle3);
        ahead();
        id[1] = 4;
      }
      id[0]= 3;
    } else if (receiveValue == "869376052") {
      Serial.println("\tReceived_P4");
      if (id[0] == 1) {
        if (id[1] == 2)
          left(angle1);  //or  right(90);
        if (id[1] == 3)
          left(angle2);  //or  right(45);
        if (id[1] == 4)
          left(angle3); //or no_rotate
        ahead();  //or  astern();
        id[1] = 1;
      } else if (id[0] == 2) {
        if (id[1] == 1)
          right(angle2);         //or left(45);
        if (id[1] == 3)
          left(angle2);          //or right(45);
        if (id[1] == 4)
          left(angle3);          //or no_rotate
        ahead_d();               //or  astern_();
        id[1] = 2;
      } else if (id[0] == 3) {
        if (id[1] == 1)
          right(angle2);         //or left(45);
        if (id[1] == 2)
          left(angle1);          //or right(90);
        if (id[1] == 4)
          left(angle3);          //or no_rotate
        ahead();                 //or astern();
        id[1] = 3;
      }
      id[0] = 4;
    }
Serial.println("\texit receiveValue()");
}

//----------------
//-----メイン-----
int id[] = {1,2};

void loop() {

  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value, DEC);
    Serial.println(results.value, BIN);

    String receiveValue = "";
    receiveValue = (String)results.value;
    //Received_P :　これから向かう点
    //id[0]          :　今いる点
    //id[1]          :　前にいた点
    Serial.println("enter receiveIR()");
    receiveIR(receiveValue,id);
    Serial.println("exited receiveIR()");
    Serial.println("enter brake()");
    brake();
    Serial.println("exited brake()");
    Serial.println("dousa syuuryou desu.\n");
    Serial.println("ima P"+(String)id[0]+"ni irun.");
    Serial.println("P"+(String)id[1]+"kara kitan.");
    Serial.println("----------");

    irrecv.resume();
  }
  delay(1000);
  
  Serial.println("speedL:"+(String)speedL+" speedR:"+(String)speedR);
  
}
//---------------
