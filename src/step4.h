#include <stdint.h>
#include <stdlib.h>
#include <arduino.h>

#define LH(cdata) (((cdata)? HIGH: LOW))

class byj {
  private:
    int _ports[4];
  public:
    const int min_delay = 2;
    const int def_delay = 2;
    const double min_speed = 0.00001;

    void init(int *ps);
    void onestep(int dir);
    void mstep(int val);
  protected:
   
  public:
    byj(int *ps);
    byj();
    ~byj() {};

    void init();
    void go(double speed, uint16_t distance); // angle
    void stop() { onestep(0); };
  /////////////////////////////////////////////////////////////  
    void test();
};

class step4_job : byj {
private:
  int delay; // speed
  uint16_t delay_cnt; // scan period * delay_cnt, ... delay_cnt = delay; delay_cnt--
  int dir;

public:
  int speed;
  uint16_t distance; // distance--

public:
  step4_job(int *ps);
  ~step4_job();

};

struct byj_job {
  byj *motor;
  int dir;
  int delay; // speed
  uint16_t delay_cnt; // scan period * delay_cnt, ... delay_cnt = delay; delay_cnt--
  uint16_t distance; // distance--

  int isalive() { return (motor)? 1 : 0; };
  void close() { motor = (byj*)0; }
  void set(struct byj_job *ss);
  void onestep();
};

#define MAX_JOB 16
#if 0
struct byj_job jobs_table[MAX_JOB];

int get_freetab(); // -1 fail
int set_tab(struct cmd_step*); // -1 fail
void init_tab();
void scan_tab(); // timer interrupt
#endif

class scanjobs {
private:
  static int _initialized;
  static struct byj_job _cmds[MAX_JOB];
  void init_tab();
public:
  scanjobs();
  ~scanjobs();

  int set_tab(struct byj_job*);
  static void scan();
};



