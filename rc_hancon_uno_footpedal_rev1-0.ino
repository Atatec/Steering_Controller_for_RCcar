/*
 * foot pedal test
 * DAコンバータmp4726使用
 * アクセル、ブレーキテスト
 * 2021/10/25
 * copy right 2021 Atatec
 */

#include<Wire.h>

//デバイスアドレス
uint8_t DEVICE_ADDRESS = 0x60;    //bin:0110 0000
//アクセル
int acc_analogPin = 0;
uint16_t acc_pre = 0;
uint16_t acc_ana = 0;
uint16_t acc = 0;
uint16_t adj_val = 20; //電圧降下補正：133=0.163/5*4096
//ブレーキ
int brk_analogPin = 1;
uint16_t brk_pre = 0;
uint16_t brk_ana = 0;
uint16_t brk = 0;


void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("foot pedal test");
}

void loop() {
  //アクセル、ブレーキのアナログ値を受信
  acc_ana = analogRead(acc_analogPin);
  brk_ana = analogRead(brk_analogPin);

  //データ更新,更新率：20%
  //フィルター & アナログ分解能（1024）→i2c分解能（4096）に変換
  acc = 0.1 * acc_pre + 0.9 * 4 * acc_ana;
  brk = 0.1 * brk_pre + 0.9 * 4 * brk_ana;
  
  //前回値更新
  acc_pre = acc;
  brk_pre = brk;
  
  //データ確認
/*  Serial.print("acc= ");
  Serial.print(acc);
  Serial.print("\(");
  Serial.print(acc / 4095.0 * 5);
  Serial.print("V\)");
  Serial.print(", brk= ");
  Serial.print(brk);
  Serial.print("\(");
  Serial.print(brk / 4095.0 * 5);
  Serial.print("V\)");*/
  
  //上下限リミッタ
  if (acc < 229) acc = 229;
  if (acc > 2296) acc = 2296;
  if (brk < 393) brk = 393;
  if (brk > 2523) brk = 2523;

  //プロポレンジに変換
  acc = map(acc, 229, 2296, 818, 1464);
  brk = map(brk, 393, 2523, 1464, 1971);

  //変換後データ確認
/*  Serial.print("  ,after acc= ");
  Serial.print(acc);
  Serial.print("\(");
  Serial.print(acc / 4095.0 * 5);
  Serial.print("V\)");
  Serial.print(", brk= ");
  Serial.print(brk);
  Serial.print("\(");
  Serial.print(brk / 4095.0 * 5);
  Serial.println("V\)");
  Serial.println();*/

  //電圧降下補正
  acc = acc + adj_val;
  brk = brk + adj_val;
  
  //ブレーキon検出　ペダル開度5％でon検出
  if (brk > (1463 * 1.05)) {
    //i2cデータ送信
    data_send(DEVICE_ADDRESS, brk);
    Serial.print("B");
  } else {  //ブレーキoff時はブレーキ値は無視してアクセル制御
    //i2cデータ送信
    data_send(DEVICE_ADDRESS, acc);
  }
/*
  //データ確認
  Serial.print("acc= ");
  Serial.print(acc);
  Serial.print("\(");
  Serial.print(acc / 4095.0 * 5);
  Serial.print("V\)");
  Serial.print("brk= ");
  Serial.print(brk);
  Serial.print("\(");
  Serial.print(brk / 4095.0 * 5);
  Serial.println("V\)");
  Serial.println();*/
}

int data_send(uint8_t address, uint16_t data) {
  Wire.beginTransmission(address);
  Wire.write((data >> 8) & 0x0F);
  Wire.write(data);
  Wire.endTransmission();
}
