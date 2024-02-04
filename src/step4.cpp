
#include "step4.h"

const uint16_t EV[] = { 0x1,0x3,0x2,0x6,0x4,0xc,0x8,0x9,0x0 };

byj::byj( int *v ) {
  if( !v ) {
    //throw "need motor 4 port";
    for(int i=0;i<4;i++) {
      this->_ports[i] = -1;
    }    
    return;
  }
  for(int i=0;i<4;i++) {
    this->_ports[i] = v[i];
  }

  this->init(v);
}

void byj::init(int *ps) {
  if( ps ) {
    pinMode(this->_ports[0],OUTPUT);
    pinMode(this->_ports[1],OUTPUT);
    pinMode(this->_ports[2],OUTPUT);
    pinMode(this->_ports[3],OUTPUT);
  } 
}

#define TEST 0

#ifndef LH  
#define LH(cdata) (((cdata)? HIGH: LOW))
#endif

void byj::mstep(int pd) {
  for( int k=0,val=0x8; k<4; k++, val >>= 1) {
    digitalWrite((uint8_t)this->_ports[k], LH(pd & val));
  }
}

void byj::onestep(int dir) {
  const int cycle = 8;

  if( dir == 0 ) { // stop
    mstep(EV[8]);
  } else
  if( dir > 0 ) {
    for( int x=0; x<cycle; x++) {
      mstep(EV[x]);
      delay(min_delay); // DE
    
    }
  } else {
    for( int x=cycle-1; x>=0; x--) {
      mstep(EV[x]);
      delay(min_delay); // DE
    }
  }
}

#if 0
void byj::onestep(int dir) {
  const uint16_t EV[] = { 0x1,0x3,0x2,0x6,0x4,0xc,0x8,0x9,0x0 };
  const int cycle = 8;

  if( dir == 0 ) { // stop
    for( int k=0,val=0x8; k<4; k++,val >>= 1) {
      digitalWrite((uint8_t)this->_ports[k], LH(EV[8] & val)); // set LOW to all pin 
    }
  } else
  if( dir > 0 ) {
    for( int x=0; x<cycle; x++) {
      for( int k=0,val=0x8; k<4; k++, val >>= 1) {
        digitalWrite((uint8_t)this->_ports[k], LH(EV[x] & val));
      }
      delay(min_delay); // DE
    
    }
  } else {
    for( int x=cycle-1; x>=0; x--) {
      for( int k=0,val=0x8; k<4; k++,val >>= 1) {
        digitalWrite((uint8_t)_ports[k], LH(EV[x] & val));
      }
      delay(min_delay); // DE
    }
  }
}
#endif

void byj::go(double speed, uint16_t distance) { 
  int dir = 1;
  uint32_t de;

  if(speed < 0.) {
    speed = -speed;
    dir = -1;
  }
  if( speed > 1. ) speed = 1.0;

  speed = min(speed, 1);

  if( speed < min_speed) {
    delay(distance * 10);
    return;
  }

  de = (uint32_t)(double)(1/(speed));  
  
  while(distance-- > 0) {
    onestep(dir);
    delay(de);
  }
}

void byj::test() {
  #if 0
  int cnt=0;
  int de=5;
  while(cnt++ < 512) {
    this->onestep(1);
    delay(de);
  }
  
  delay(1000);

  while(cnt-- > 0) {
    this->onestep(-1);
    delay(de);
  }
  #else
  go(0.95,512);
  delay(1000);
  go(-0.007,51);
  #endif
}

step4_job::step4_job(int *ps) : byj(ps) {
  _speed = 0;
  _distance = 0;

  _dir = 0;
  _delay = _delay_cnt = 0;
}

step4_job::~step4_job() {
  _dir = 0;
}


void step4_job::seq_step() { // called timer interrupt
  noInterrupts(); // inhebit external interrupt

  // stoped or nothing to do
  if( _dir == 0 ) goto seq_stop_return;

  // in process
  if( _step_no != -1 ) {
    if( _step_no < 0 || _step_no > 7 ) {
      goto seq_stop_return; // error
    }
    mstep(_step_no++);
    if( _step_no > 7 ) {
      // ending mstep
      _step_no = -1;

    }
    goto seq_stop_return;
  }

  if( --_delay_cnt > 0 ) {
    goto seq_stop_return;
  } else {
    // restart step
    _step_no = 0;
    _delay_cnt = _delay;
  }

  if( --_distance > 0 ) {
    // calc, establish plan
    // delay, delay_cnt
  }


seq_stop_return:
  interrupts(); // allow interrupt
}

void step4_job::go(double speed, uint16_t distance) {
  noInterrupts();

  _dir = (_speed < 0)? -1: (abs(speed) < min_speed)? 0 : 1;
  _speed = abs(speed);
  _distance = distance;

  interrupts();
}

step4_job *__step4s[MAX_STEP4_MOTOR] = { 0,0,0,0, };

void scan_step4() {
  for(int i=0;i < MAX_STEP4_MOTOR; i++) {
    if( __step4s[i] == (step4_job *)0 ) {
      return;
    }
    __step4s[i]->seq_step();
  }
}

int append_step4(class step4_job *sj) {
  for(int i=0;i < MAX_STEP4_MOTOR; i++) {
    if( __step4s[i] == (step4_job *)0 ) {
      __step4s[i] = sj;
      return i;
    }
  }
  return -1;
}
