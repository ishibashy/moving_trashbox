#include <IRremote.h>

// モータ
//左
const int motorL1 = 8;
const int motorL2 = 9;
const int PWM_motL = 10;
//右
const int motorR1 = 11;
const int motorR2 = 12;
const int PWM_motR = 5;
//スピード
int speedL = 120;
int speedR = 150;
//IRモジュール
const int IRMDL = 7;
IRrecv irrecv(IRMDL);
decode_results results;
//フォトインタラプタ、change回数
int changeL = 0;
int changeR = 0;
int valL1 = HIGH;
int valL2 = HIGH;
int valR1 = HIGH;
int valR2 = HIGH;
const int counterL = 2;
const int counterR = 3;
//測距モジュール
int count = 0;//pin番号ではない

//ブザー
const int buzzer = 6;
int melo = 500;
//関数のID
int method_id;
//既知のIRかどうかの判別
bool knownIR = false;


//-----初期化-----
void setup() {
  // モータ
  //右
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
  //左
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  //IRモジュール
  irrecv.enableIRIn();
  //フォトインタラプタ
  pinMode(counterL, INPUT);
  pinMode(counterR, INPUT);
  //ブザー
  pinMode(buzzer, OUTPUT);
  
  Serial.begin(9600);
}
//-メインで使う関数-


void photo_changing() {                    //フォトインタラプタ
  valL1 = digitalRead(counterL);
  valR1 = digitalRead(counterR);
  if(valL1 != valL2){
     valL2 = valL1;
     changeL++;
  }
  if(valR1 != valR2){
     valR2 = valR1;
     changeR++;
  }
  if (changeR > changeL) {
    speedR -= 1;
    if(speedR < 0)
      speedR = 0;
    speedL = 150;
  }else{
    speedL -= 1;
    if(speedL < 0)
      speedL = 0;
    speedR = 150;
  }
    Serial.println("changeL:"+(String)changeL+" changeR:"+(String)changeR);
    Serial.println("speedL:"+(String)speedL+" speedR:"+(String)speedR);
}


void cw(int melo){                            //トンツートンツー・ー・ー
  int melo_speed = 1;
  digitalWrite(buzzer,HIGH);
  delay(melo * melo_speed);
  digitalWrite(buzzer,LOW);
  delay(100 * melo_speed);
}

void CBuzzer(bool b) {                       //ブザー
  Serial.println("\t\t\tentered CBuzzer("+(String)b+")");
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
    cw(100);cw(100);//cw(100);//ブザー1
//    delay(200);
//    cw(300);cw(300);cw(300);
//    delay(200);
//    cw(100);cw(100);cw(100);
    b = false;
  }else{
    cw(300);//cw(300);cw(100);//ブザー2
//    delay(200);
//    cw(300);cw(300);cw(300);
    b = true;
  }
  Serial.println("\t\t\texit CBuzzer("+(String)b+")");
}

void barricade_check() {                    //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;
  Serial.println("\t\t"+(String)distance+"cm");

  if (0 < distance && distance < 40) 
    count++;
  else
    count = 0;
      
  if(count > 4) {
    Serial.println("\t\t!!!STOP!!!");
    int tmpL = changeL;
    int tmpR = changeR;
    brake();
    Serial.println("\t\tenter CBuzzer(true)");
    CBuzzer(true);//ブザー1
    Serial.println("\t\texited CBuzzer(true)");
    while (count > 0){
      distance = (6762 / (analogRead(0) - 9)) - 4;
      Serial.println("\t\t\t"+(String)distance+"cm");
      if (distance > 39 || distance < 0 )
        count--;
      delay(100);
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
  analogWrite(PWM_motL, speedL);
  analogWrite(PWM_motR, speedR);
}

void circle(int angle) {                    //回転
  //analogWrite(PWM_motL, 150);
  //analogWrite(PWM_motR, 150);
  Serial.println("\t\t\tentered circle("+(String)angle+")");
  double circleBaisuu = 7;
  if (angle == 180)
//    while (1) {
//      motor_speed();
//      if (changeL > 19 || changeR > 19)
//          break;
//    }
//    while (changeL < 2 * circleBaisuu  && changeR < 2 * circleBaisuu) {
//      photo_changing();
//    }
      while ( changeL < 18 ){
        valL1 = digitalRead(counterL);
        valR1 = digitalRead(counterR);
        if(valL1 != valL2){
          valL2 = valL1;
          changeL++;
        }
          analogWrite(PWM_motL, 0);
          analogWrite(PWM_motR, 0);
          delay(100);
          //koko
          analogWrite(PWM_motL, 150);
          analogWrite(PWM_motR, 150);
          delay(100);
//        valL1 = digitalRead(counterL);
//        valR1 = digitalRead(counterR);
//        if(valL1 != valL2){
//          valL2 = valL1;
//          changeL++;
//          analogWrite(PWM_motL, 0);
//          analogWrite(PWM_motR, 0);
//          delay(100);
//          Serial.println("changeL:"+(String)changeL+" changeR:"+(String)changeR);
        //}
//        if(valR1 != valR2){
//          valR2 = valR1;
//          changerR++;
//          analogWrite(PWM_motL, 0);
//          analogWrite(PWM_motR, 0);
//          delay(1000);
//        }
//        analogWrite(PWM_motL, 150);
//        analogWrite(PWM_motR, 150);
      }
  else if (angle == 135)
    while (changeL < 1.5 * circleBaisuu && changeR < 1.5 * circleBaisuu) {
      photo_changing();
    }
  else
    while (changeL < 1 * circleBaisuu && changeR < 1 * circleBaisuu) {
      photo_changing();
      
    }
  changeL = changeR = 0;
  Serial.println("\t\t\texit circle("+(String)angle+")");
//  digitalWrite(motorL1, LOW);
//  digitalWrite(motorL2, LOW);
//  digitalWrite(motorR1, LOW);
//  digitalWrite(motorR2, LOW);
    digitalWrite(motorL1,1);
    digitalWrite(motorL2,1);
    digitalWrite(motorR1,1);
    digitalWrite(motorR2,1);
  delay(1000);
}

void ahead() {                              //前進
  Serial.println("\t\tentered ahead()");
  method_id = 0;
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter while(1)");
  //while (changeL < 30 && changeR < 30) {
  while (changeL < 342 && changeR < 342) {
    
    motor_speed();
    photo_changing();
    barricade_check();
    delay(100);
  }
  Serial.println("\t\texited while(1)");
  Serial.println("\t\texit ahead())");
  return;
}


void ahead_d() {                            //前進斜め
  Serial.println("\t\tentered ahead_d()");
  method_id = 1;
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter while(1)");
 // while (changeL < 964 && changeR < 964) {
    while (changeL < 482 && changeR < 482) {
    motor_speed();
    photo_changing();
    barricade_check();
  }
  Serial.println("\t\texited while(1)");
  Serial.println("\t\texit ahead_d())");
  return;
}

void astern(int changechange) {             //後進
  Serial.println("\t\tentered astern("+(String)changechange+")");
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  Serial.println("\t\tThe operation by inertia:"+(String)changechange);
  Serial.println("\t\tenter while(1)");
  while (1) {
    motor_speed();
    photo_changing();
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
  if(knownIR == false){
    Serial.println("\texit brake()for a unknown IR");
    return;
  }
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  changeL = changeR = 0;
  Serial.println("wait for 3 seconds.");
  delay(3000);
  int changechange = changeL;
  if (changechange < changeR)
    changechange = changeR;
  changeL = changeR = 0;
  Serial.println("\tenter astern("+(String)changechange+")");
  astern(changechange);
  Serial.println("\texited astern("+(String)changechange+")");
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  Serial.println("\texit brake()");
}

void left(int left) {                       //左旋回
  Serial.println("\t\tentered left("+(String)left+")");
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter circle("+(String)left+")");
  circle(left);
  Serial.println("\t\texited circle("+(String)left+")");
  Serial.println("\t\texit left("+(String)left+")");
}
void right(int right) {                     //右旋回
  Serial.println("\t\tentered right("+(String)right+")");
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter circle("+(String)right+")");
  circle(right);
  Serial.println("\t\texited circle("+(String)right+")");
  Serial.println("\t\texit right("+(String)right+")");
}

void receiveIR(String receiveValue,int *id){    //受信
Serial.println("\tentered receiveIR()");
changeL = changeR = 0;
const int angle1 = 90;
const int angle2 = 135;
const int angle3 = 180;
knownIR = true;
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
    //} else if (receiveValue == "4012159527") {
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
        Serial.println("\texited ahead()");
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
    } else {
      knownIR = false;
      Serial.println("\tunknown IR received ");
    }
Serial.println("\texit receiveValue()");
}

//----------------
//-----メイン-----
int id[] = {1,2};

void loop() {
//  delay(2000);
//  digitalWrite(motorL1, HIGH);
//  digitalWrite(motorL2, LOW);
//  analogWrite(PWM_motL, speedR);
//  digitalWrite(motorR1, LOW);
//  digitalWrite(motorR2, HIGH);
//  analogWrite(PWM_motR, speedL);
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
  Serial.println(".");
  
}
//---------------
