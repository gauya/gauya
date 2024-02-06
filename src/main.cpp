#include <Arduino.h>
#include "step4.h"
#include "ds3231.h"

//long time;

#include <TimerOne.h>

void timer_func() {
    //
    static int on = 1;
    static unsigned long cnt = 0;
    if( cnt++ > 1000 ) {
      digitalWrite(LED_BUILTIN,on);
      on = !on;
      cnt = 0;
    }

    scan_step4();
};

void ex_isr() {
  // stop or dir reverse
  // interrupts(), nointerrupts()

  Serial.print(":");
}

int interruptPin = 2;
int motor1_pins[] = { 11,10,9,8,0 };
int motor2_pins[] = { 4,5,6,7,};
//byj *B;
step4_job *SA,*SB;

void setup() {
  Serial.begin(115200);
  Serial.println("Hello");

  pinMode(LED_BUILTIN,OUTPUT); // led
  
  Timer1.initialize(1000); // 1ms
  Timer1.attachInterrupt(timer_func);

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), ex_isr, CHANGE);  // RISING,FALLING, HIGH, LOW

  //B = (byj*)new byj((int *)motor1_pins);
  
  SB = new step4_job((int *)motor1_pins);
  SA = new step4_job((int *)motor2_pins);

  int id1 = append_step4(SB);
  int id2 = append_step4(SA);

  ds3231_setup();

  char buf[80];
  sprintf(buf,"id = %d/%d, szLint = %d, byj=%d, step4_job=%d",id1, id2, sizeof(int), sizeof(class byj), sizeof(step4_job));
  Serial.println(buf);

}

void loop() {
  static int cnt=0;
  SB->test();
  SA->test();

  struct tm t;
  read_ds3231(&t);

  char buf[80];
  sprintf(buf,"cnt=20%x %02x.%02x.%02x %02x:%02x:%02x",cnt++, t.tm_year,t.tm_mon,t.tm_mday,t.tm_hour,t.tm_min,t.tm_sec);

  Serial.println(buf);
}

