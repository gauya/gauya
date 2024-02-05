
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

void byj::go(int speed, uint16_t distance) { 
  int dir = 1;
  uint32_t de;

  if(abs(speed) < min_speed) {
    delay(distance * 10);
    dir = 0;
    return;
  }
  if( speed > min_speed ) {
    speed = min_speed;
  } else 
  if( speed < -min_speed) {
    speed = -speed;
    dir = -1;
  }

  de = (uint32_t)(1000./(speed));  
  
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
  go(950,120);
  delay(1000);
  go(-600,120);

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
  if( _dir != -1 && _dir != 1 ) {
    _dir = 0;
    goto seq_stop_return;
  }

  if( _mdelay > 0 ) {
    _mdelay--;
    goto seq_stop_return;
  }
  if( _step_no != -1 ) {
    if( _step_no < 0 || _step_no > 7 ) {
      _step_no = -1;
      goto seq_stop_return; // error
    }
    mstep(EV[_step_no]);
    _step_no += _dir;
    _mdelay = min_delay;
    if( _step_no > 7 || _step_no < 0 ) {
      // ending mstep
      _step_no = -1;
      _delay_cnt = _delay;
    } else {
    }
    goto seq_stop_return;
  }

  if( _delay_cnt > 0 ) {
    _delay_cnt--;
    goto seq_stop_return;
  } else {
    // restart step
    if( _distance > 0 ) {
      // calc, establish plan
      // delay, delay_cnt
      _distance--;
      _step_no = (_dir == -1)? 7: 0;
      _delay_cnt = _delay;
      _mdelay = min_delay;
    } else {
      _dir = 0;
    }
  }

seq_stop_return:
  interrupts(); // allow interrupt
}

void step4_job::go(int speed, uint16_t distance) {
  while( !isstop() ) {
    delay(1);
  }
  
  noInterrupts();

  //_dir = (_speed < -min_speed)? -1: (speed > min_speed)? 1 : 0; // error
  _dir = (abs(_speed) < min_speed)? 0 : (speed > min_speed)? 1 : -1;

/*
  char buf[80];
  sprintf(buf,"speed=%d/%d d=%u dir=%d\n",speed,min_speed, distance,_dir);
  Serial.println(buf);
*/

  _speed = abs(speed);
  _distance = distance;
  _delay = 10000 / (_speed + min_delay);
  if(_delay < min_delay) _delay = min_delay;
  //_delay = 100;
  _delay_cnt = _delay;
  _mdelay = 0;
  _step_no = (_dir == 0)? -1: (_dir == -1)? 7 : 0;

  interrupts();
}

void step4_job::info() const {
  char buf[180];
  sprintf(buf,"step4info: id(%d) dir(%d) speed(%d) distance=%d delay=%d",_id,_dir,_speed,_distance, _delay);
  Serial.println("step4info:");
}

void step4_job::stop() { // not pause
  noInterrupts();

  _dir = 0;
  _speed = 0;
  _distance = 0;
  _delay = _delay_cnt = _mdelay = 0;
  _step_no = -1;

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

int append_step4( step4_job *sj) {
  for(int i=0;i < MAX_STEP4_MOTOR; i++) {
    if( __step4s[i] == (step4_job *)0 ) {
      __step4s[i] = sj;
      sj->_id = i;
      return i;
    }
  }
  return -1;
}
