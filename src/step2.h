#include <arduino.h>

class step2 {
private:
    int *_pins; // en, dir, pls, 0;
public:
    const int _min_us = 100;
public:
    step2(int *pins=0);
    ~step2(); 

    void mstep(int dir);
    void go(double speed, uint32_t distance);
};