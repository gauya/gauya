#include <stdint.h>
#include <stdlib.h>
#include <arduino.h>

#ifndef __STEP4_H__
#define __STEP4_H__

#define LH(cdata) (((cdata)? HIGH: LOW))

class byj {
  private:
    int _ports[4];
  public:
    static const int min_delay = 2;
    static constexpr double min_speed = 0.00001f;
    
    void init(int *ps);
    void onestep(int dir);
    void mstep(int val);
  protected:
   
  public:
    byj(int *ps);
    ~byj() {};

    void init();
    virtual void go(double speed, uint16_t distance); // angle
    void stop() { onestep(0); };
    operator int *() { return (int*)_ports; }
  /////////////////////////////////////////////////////////////  
    void test();
};

class step4_job : public byj {
private:
  uint16_t _delay; // from speed
  uint16_t _delay_cnt; // check per 100 usec, scan period * delay_cnt, ... delay_cnt = delay; delay_cnt--
  int _step_no; // 0~7 , -1 : default

public:
  int _dir; //-1, 0(stoped), 1
  double _speed; // -1 ~ 1
  uint16_t _distance; // distance--

public:
  step4_job(int *ps);
  ~step4_job();

  void seq_step();
  void go(double speed, uint16_t distance); // timer interrupt will handle
};

#define MAX_STEP4_MOTOR 4

void scan_step4();
int append_step4(class step4_job *sj) ;

#endif // __STEP4_H__

