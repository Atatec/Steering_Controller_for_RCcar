/* 
 *  ステアリングエンコーダ出力テスト
 *  2021/11/1
 *  copy right 2021 Atatec
 *  
 *  Encoder Library - Basic Example
 * http://www.pjrc.com/teensy/td_libs_Encoder.html
 *
 * This example code is in the public domain.
 */

#include <Encoder.h>
#include <Wire.h>

//デバイスアドレス
uint8_t DEVICE_ADDRESS = 0x60;    //bin:0110 0000

//ステアリング
int str_analogPin = 0;
int16_t zeroPosition = 1338; // 1337.7=1.694/5*4096
int16_t newPosition = 0;
int16_t newPosition_output = 0;
int16_t oldPosition  = -999;

// Change these two numbers to the pins connected to your encoder.
//   Best Performance: both pins have interrupt capability
//   Good Performance: only the first pin has interrupt capability
//   Low Performance:  neither pin has interrupt capability
Encoder myEnc(2, 3);
//   avoid using pins with LEDs attached

void setup() {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("Encoder Output Test:");
}

void loop() {
  newPosition = myEnc.read();
  if (newPosition != oldPosition) {
    oldPosition = newPosition;
    Serial.println(newPosition);
  }
  //右ステア
  if (newPosition >= 0) {
    if (newPosition > 400) newPosition = 400; //上限リミッタ
    //プロポレンジに変換
    newPosition_output = map(newPosition, 0, 400, zeroPosition, 2269); // 2269=2.770/5*4096
  } else if (newPosition < 0) { //左ステア
    if (newPosition <= -400) newPosition = -400;  //下限リミッタ
    //プロポレンジに変換
    newPosition_output = map(newPosition, -400, 0, 448, zeroPosition - 1); //448=0.547/5*4096
  } else if (newPosition == 0) {
    newPosition_output = zeroPosition;
  }
  data_send(DEVICE_ADDRESS, newPosition_output);
  Serial.print(" newPosition_output=");
  Serial.println(newPosition_output);
}

int data_send(uint8_t address, uint16_t data) {
  Wire.beginTransmission(address);
  Wire.write((data >> 8) & 0x0F);
  Wire.write(data);
  Wire.endTransmission();
}
