#include "step2.h"

step2::step2(int *ps) {
    _pins = ps;
}

step2::~step2() {
    //
}
void step2::mstep(int dir) {
    if( !_pins ) return;

    digitalWrite(_pins[0],1); // EN
    digitalWrite(_pins[1],1); // DIR
    digitalWrite(_pins[2],1); // PLS

    delayMicroseconds(_min_us);

    digitalWrite(_pins[0],0); // EN
    digitalWrite(_pins[1],0); // DIR
    digitalWrite(_pins[2],0); // PLS

    delayMicroseconds(_min_us);
}

void step2::go(double speed, uint32_t distance) {
    int dir = (speed >= 0.)? 1 : 0;
    if( speed < 0. ) speed = -speed;

    while(distance-- > 0) {
        this->mstep(dir);

    }
}