
//スピード
int speedL = 255;//後で直進する比を実測、255:237とか
int speedR = 255;

//フォトインタラプタ、change回数
int volatile changeL = 0;
int volatile changeR = 0;

//-----初期化-----
void setup() {

  //フォトインタラプタ
  attachInterrupt(1, photo_changingL, CHANGE);
  attachInterrupt(0, photo_changingR, CHANGE);

  
  Serial.begin(9600);
}
//----------------

//--割り込み関数--
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
//----------------
void loop(){
  delay(1000);
  Serial.print("speedL:");
  Serial.print(speedL);
  Serial.print(" speedR:");
  Serial.println(speedR);
}
