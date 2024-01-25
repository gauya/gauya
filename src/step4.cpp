
#include "step4.h"

byj::byj( int *v ) {
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

void byj::onestep(int dir) {
  const uint16_t EV[] = { 0x1,0x3,0x2,0x6,0x4,0xc,0x8,0x9,0x0 };
  const int cycle = 8;

#ifndef LH  
#define LH(cdata) (((cdata)? HIGH: LOW))
#endif

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
//#undef LH  
}

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
  go(-0.007,512);
  #endif
}

void byj_job::set( struct byj_job *sp) {
  motor = sp->motor;
  dir = sp->dir;
  delay = delay_cnt = sp->delay;
  distance = sp->distance;
}

void byj_job::scan() {
  if( ! isalive() ) return;

  if(--delay_cnt <= 0) {
    // some one step process
    // ...

    if(--distance <= 0) {
      motor = 0; // remove job;
      //delay = delay_cnt = 0; distance = 0;
    }
  }
}


scanjobs::scanjobs() {

}

scanjobs::~scanjobs() {
}

void scanjobs::init_tab() {
  struct byj_job *mt = get_tab();
  for( int i=0; i < MAX_JOB; i++ ) {
    mt[i].close();
  }
  return;
}

void scanjobs::scan_tab() {
  struct byj_job *sc = get_tab(); //_cmds;
  for( int i=0; i < MAX_JOB; i++,sc++ ) {
    if( sc->isalive() ) {
      sc->scan();
    }
  }
  return;
}

int scanjobs::set_tab( struct byj_job * ss) {
  if( !ss ) 
    return -1;
  struct byj_job* lst = get_tab();
  for( int i=0; i < MAX_JOB; i++ ) {
    if( ! lst[i].isalive() ) {
      lst[i].set(ss);
      return i;
    }
  }
  return -1;
}

void jobinit() {
  scanjobs::init_tab();
}
