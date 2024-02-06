#include "ds3231.h"

void ds3231_setup() {
  Wire.begin();
  Wire.beginTransmission(104); // DS3231로 전송모드 시작 (DS3231 어드레스는 104 이다)
  Wire.write(0); // 변경할 DS3231의 레지스터리 주소를 전송. (0은 초 단위 레지스티, 1은 분 단위 레지스터리)

  Wire.write(0x0); // Second 변경
  Wire.write(0x44); // Minute 변경
  Wire.write(0x19); // Hour 변경
  Wire.write(0x6); // Day 변경
  Wire.write(0x37); // Date 변경
  Wire.write(0x2); // Month 변경
  Wire.write(0x24); // Year 변경

  Wire.endTransmission(); // DS3231로 전송을 마침.
  
  delay(1000);
}

int read_ds3231( struct tm *t) {
  Wire.beginTransmission(104); // DS3231로 전송모드 시작 (DS3231 어드레스는 104 이다)
  Wire.write(0); // 읽어올 DS3231의 레지스터리 주소를 전송. (0은 초 단위 레지스티, 1은 분 단위 레지스터리)
  Wire.endTransmission(); // DS3231로 전송을 마침.
  
  Wire.requestFrom(104,7);
  t->tm_sec = Wire.read(); 
  t->tm_min = Wire.read(); 
  t->tm_hour = Wire.read(); 
  t->tm_mday = Wire.read(); 
  t->tm_yday = Wire.read(); 
  t->tm_mon = Wire.read(); 
  t->tm_year = Wire.read(); 

  return 0;
}