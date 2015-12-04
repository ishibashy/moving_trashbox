#include <IRremote.h>

// モータ
//左(ピン番号)
const int motorL1 = 8;
const int motorL2 = 9;
const int PWM_motL = 10;

//右(ピン番号)
const int motorR1 = 11;
const int motorR2 = 12;
const int PWM_motR = 5;

//スピード
int speedL = 255;
int speedR = 255;

//IRモジュール
const int IRMDL = 7;//ピン番号
IRrecv irrecv(IRMDL);
decode_results results;

//フォトインタラプタ
//ピン番号
const int counterL = 2;
const int counterR = 3;

//change回数
int changeL = 0;
int changeR = 0;

//状態(HIGH/LOW)
int valL1 = HIGH;
int valL2 = HIGH;
int valR1 = HIGH;
int valR2 = HIGH;

//フォトインタラプタ、change回数（割り込み）
//int volatile _changeL = 0;
//int volatile _changeR = 0;

//測距モジュール
int count = 0;

//ブザー
const int buzzer = 6;//ピン番号
int melo = 500;//音の長さ

//関数のID、直進か前進斜めか判別
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
//----------------


//-メインで使う関数-
void photo_changing() {                    //フォトインタラプタ
  //フォトインタラプタの状態を代入
  valL1 = digitalRead(counterL);
  valR1 = digitalRead(counterR);

  //値が変化していたら1カウント
  if (valL1 != valL2) {
    valL2 = valL1;
    changeL++;
  }
  if (valR1 != valR2) {
    valR2 = valR1;
    changeR++;
  }
  Serial.println("changeL:" + (String)changeL + " changeR:" + (String)changeR + " speedL:" + (String)speedL + " speedR:" + (String)speedR);
}

void cw(int melo) {                        //ブザー1回の音
  int melo_speed = 1;//スピード、小さいほど高速
  digitalWrite(buzzer, HIGH);
  delay(melo * melo_speed);
  digitalWrite(buzzer, LOW);
  delay(100 * melo_speed);
}

void CBuzzer(bool b) {                     //ブザー
  Serial.println("\t\t\tentered CBuzzer(" + (String)b + ")");
  if (b) {
    //ブザー1(急停止)
    cw(100); cw(100); cw(100);
    delay(200);
    cw(300); cw(300); cw(300);
    delay(200);
    cw(100); cw(100); cw(100);

    b = false;
  } else {
    //ブザー2(再発進)
    cw(300); cw(300); cw(100);
    delay(200);
    cw(300); cw(300); cw(300);

    b = true;
  }
  Serial.println("\t\t\texit CBuzzer(" + (String)b + ")");
}

void barricade_check() {                   //障害物検知
  int distance;
  distance = (6762 / (analogRead(0) - 9)) - 4;//障害物までの距離をcm単位で計算
  Serial.println("\t\t" + (String)distance + "cm");

  if (0 < distance && distance < 49) //この閾値により障害物を判定
    count++;
  else
    count = 0;

  if (count > 2) {                   //閾値超えを連続でカウントしたら急停止
    Serial.println("\t\t!!!STOP!!!");
    //移動量を一時退避
    int tmpL = changeL;
    int tmpR = changeR;

    count = 5;
    brake();
    Serial.println("\t\tenter CBuzzer(true)");
    CBuzzer(true);//ブザー1
    Serial.println("\t\texited CBuzzer(true)");
    //閾値超えを連続でカウントしたら再発進
    while (count > 0) {
      distance = (6762 / (analogRead(0) - 9)) - 4;
      Serial.println("\t\t\t" + (String)distance + "cm");
      if (distance > 49 || distance < 0 )
        count--;
      else
        count = 10;
      delay(100);
    }

    Serial.println("\t\t!!!RESTART!!!");
    Serial.println("\t\tenter CBuzzer(false)");
    CBuzzer(false);//ブザー2
    Serial.println("\t\texited CBuzzer(false)");
    //退避していた移動量を代入
    changeL = tmpL;
    changeR = tmpR;

    if (method_id == 0)
      ahead();
    else
      ahead_d();
  }
}

void motor_speed() {                       //モータ速度更新
  analogWrite(PWM_motL, speedL);
  analogWrite(PWM_motR, speedR);
}

void minuteMovement() {                    //微小移動
  photo_changing();
  speedL = speedR = 0;
  Stop();
  motor_speed();
  delay(50);
  photo_changing();
  speedL = speedR = 255;
  motor_speed();
  delay(15);
}

void circle(int angle) {                   //回転
  Serial.println("\t\t\tentered circle(" + (String)angle + ")");
  //仮引数によって回転量を区別
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  if (angle == 180)
    while ( changeL < 40 ) {
      minuteMovement();
    }
  else if (angle == 135)
    while (changeL < 30) {
      minuteMovement();
    }
  else
    while (changeL < 20) {
      minuteMovement();
    }
  changeL = changeR = 0;
  Serial.println("\t\t\texit circle(" + (String)angle + ")");
  Stop();
  delay(1000);
}

void ahead() {                             //前進
  Serial.println("\t\tentered ahead()");
  method_id = 0;
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  //直進のための微調整
  delay(200);
  speedR = 255;
  speedL = 0;
  motor_speed();
  delay(30);
  speedR = 255;
  speedL = 255;
  motor_speed();
  delay(450);
  speedL = 100;
  speedR = 110;
  motor_speed();
  
  Serial.println("\t\tenter while(1)");
  while (changeL < 10 && changeR < 10) {
    motor_speed();
    photo_changing();
    barricade_check();
    delay(10);
  }
  Serial.println("\t\texited while(1)");
  Serial.println("\t\texit ahead())");
  return;
}


void ahead_d() {                           //前進斜め
  Serial.println("\t\tentered ahead_d()");
  method_id = 1;
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter while(1)");
  //直進のための微調整
  delay(200);
  speedR = 255;
  speedL = 0;
  motor_speed();
  delay(30);
  speedR = 255;
  speedL = 255;
  motor_speed();
  delay(450);
  speedL = 100;
  speedR = 110;
  motor_speed();
  
  Serial.println("\t\tenter while(1)");
  while (changeL < 15 && changeR < 15) {
    motor_speed();
    photo_changing();
    barricade_check();
    delay(10);
  }
  Serial.println("\t\texited while(1)");
  Serial.println("\t\texit ahead())");
  return;
}

void astern(int changechange) {             //後進
  Serial.println("\t\tentered astern(" + (String)changechange + ")");
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedR);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedL);
  Serial.println("\t\tThe operation by inertia:" + (String)changechange);
  Serial.println("\t\tenter while(1)");
  //ブレーキから停止までの移動した量だけ後進
  while (1) {
    motor_speed();
    photo_changing();
    if (changeL >= changechange || changeR >= changechange) {
      Serial.println("\t\t\texited while(1)");
      break;
    }
  }
  Serial.println("\t\texited while(1)");
  changeL = changeR = 0;
  Serial.println("\t\texit astern(" + (String)changechange + ")");
}

void shortAstern(int time) {                //ブレーキの代わりの短い後進
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, 255);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, 255);
  delay(time);
}

void Stop() {                               //モータ停止
  digitalWrite(motorL1, 1);
  digitalWrite(motorL2, 1);
  digitalWrite(motorR1, 1);
  digitalWrite(motorR2, 1);
}

void brake() {                              //ブレーキ(短い後進+α)
  Serial.println("\tentered brake()");
  //未知のIRの受信の場合、処理せずリターン
  if (knownIR == false) {
    Serial.println("\texit brake()for a unknown IR");
    return;
  }

  shortAstern(300);
  Stop();
  //ブレーキから停止までの移動した量を後進
  changeL = changeR = 0;
  Serial.println("wait for 2 seconds.");
  delay(2000);
  int changechange = changeL;
  if (changechange < changeR)
    changechange = changeR;
  changeL = changeR = 0;
  Serial.println("\tenter astern(" + (String)changechange + ")");
  astern(changechange);
  Serial.println("\texited astern(" + (String)changechange + ")");
  Stop();

  Serial.println("\texit brake()");
}

void left(int left) {                      //左旋回
  Serial.println("\t\tentered left(" + (String)left + ")");
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, HIGH);
  digitalWrite(motorR2, LOW);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter circle(" + (String)left + ")");
  circle(left);
  Serial.println("\t\texited circle(" + (String)left + ")");
  Serial.println("\t\texit left(" + (String)left + ")");
}
void right(int right) {                    //右旋回
  Serial.println("\t\tentered right(" + (String)right + ")");
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, HIGH);
  analogWrite(PWM_motL, speedL);
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedR);
  Serial.println("\t\tenter circle(" + (String)right + ")");
  circle(right);
  Serial.println("\t\texited circle(" + (String)right + ")");
  Serial.println("\t\texit right(" + (String)right + ")");
}

void receiveIR(String receiveValue, int *id) {//受信
  Serial.println("\tentered receiveIR()");
  changeL = changeR = 0;
  const int angle1 = 90;
  const int angle2 = 135;
  const int angle3 = 180;
  knownIR = true;
  //登録IRと一致した時、旋回→前進の処理、位置idの更新
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
      if (id[1] == 2) {
        Serial.println("\tenter left(" + (String)angle3 + ")");
        left(angle3);
        Serial.println("\texited left(" + (String)angle3 + ")");
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
    } else if (id[0] == 4) {
      if (id[1] == 1)
        left(angle1);
      if (id[1] == 2)
        left(angle2);
      if (id[1] == 3)
        left(angle3);
      ahead();
      id[1] = 4;
    }
    id[0] = 3;
  } else if (receiveValue == "869376052") {
    Serial.println("\tReceived_P4");
    if (id[0] == 1) {
      if (id[1] == 2)
        left(angle1);
      if (id[1] == 3)
        left(angle2);
      if (id[1] == 4)
        left(angle3);
      ahead();
      id[1] = 1;
    } else if (id[0] == 2) {
      if (id[1] == 1)
        right(angle2);
      if (id[1] == 3)
        left(angle2);
      if (id[1] == 4)
        left(angle3);
      ahead_d();
      id[1] = 2;
    } else if (id[0] == 3) {
      if (id[1] == 1)
        right(angle2);
      if (id[1] == 2)
        left(angle1);
      if (id[1] == 4)
        left(angle3);
      ahead();
      id[1] = 3;
    }
    id[0] = 4;
  } else {
    knownIR = false;//登録IRと一致しなかった場合
    Serial.println("\tunknown IR received ");
  }
  Serial.println("\texit receiveValue()");
}
//------------------


//------メイン------
//Received_P :　これから向かう点
//id[0]      :　今いる点
//id[1]      :　前にいた点
int id[] = {1, 2};

void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    Serial.println(results.value, DEC);
    Serial.println(results.value, BIN);
    String receiveValue = "";
    receiveValue = (String)results.value;
    Serial.println("enter receiveIR()");
    receiveIR(receiveValue, id);
    Serial.println("exited receiveIR()");
    Serial.println("enter brake()");
    brake();
    Serial.println("exited brake()");
    Serial.println("dousa syuuryou desu.\n");
    Serial.println("ima P" + (String)id[0] + "ni irun.");
    Serial.println("P" + (String)id[1] + "kara kitan.");
    Serial.println("----------");

    irrecv.resume();
  }
  delay(1000);
  Serial.println(".");
}
//------------------
