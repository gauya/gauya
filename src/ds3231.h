#include <arduino.h>
#include "time.h"
#include "Wire.h"

void ds3231_setup();
int read_ds3231( struct tm *t);

