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
//フォトインタラプタ、change回数
int volatile changeL = 0;
int volatile changeR = 0;
//関数のID
int id;

//-----初期化-----
void setup() {
  delay(10000);
  
  // モータ
  //右
  pinMode(motorR1, OUTPUT);
  pinMode(motorR2, OUTPUT);
  //左
  pinMode(motorL1, OUTPUT);
  pinMode(motorL2, OUTPUT);
 
  Serial.begin(9600);

  ahead();
  
}
//----------------

void photo_changingR() {                    //フォトインタラプタ右
  changeR++;
  Serial.print(changeR);
  Serial.println("times_changed_R");
  if (changeR > changeL) {
    speedR--;
    speedL = 255;//後で直進する比を実測
  }
}
//----------------
//-メインで使う関数-


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
    if (changeL > 964 || changeR > 964)
      break;
  }
  changeL = changeR = 0;
}

void astern(int changechange) {             //後進
  Serial.println("start astern()");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, HIGH);
  analogWrite(PWM_motR, speedL);
  digitalWrite(motorL1, HIGH);
  digitalWrite(motorL2, LOW);
  analogWrite(PWM_motL, speedR);
  Serial.println("while ni hairimasu");
  while (1) {
    Serial.println("while");
    motor_speed();
    if (changeL >= changechange || changeR >= changechange)
      break;
  }
  Serial.println("break");
  changeL = changeR = 0;
}

void brake() {                              //ブレーキ
  Serial.println("start brake()");
  digitalWrite(motorR1, LOW);
  digitalWrite(motorR2, LOW);
  digitalWrite(motorL1, LOW);
  digitalWrite(motorL2, LOW);
  int changechange = changeL;
  if (changechange < changeR)
    changechange = changeR;
  changeL = changeR = 0;
  Serial.println("astern() ni hairimasu");
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
//----------------
//-----メイン-----
int id1 = 1;
int id2 = 2;
int angle1 = 90;
int angle2 = 135;
int angle3 = 180;
void loop() {

  delay(10000);
  brake();


}
//---------------
