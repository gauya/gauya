#include <arduino.h>

#include <stdio.h>
#include <stdarg.h>

#define VAPRINT_MODE 1

void _printf(const char *s,...){ 
    va_list args; 
    va_start(args, s); 
    
#if ( VAPRINT_MODE == 0 )
    int n = vsnprintf(NULL, 0, s, args); 
    char *str = new char[n+1]; 
    vsprintf(str, s, args); 
    va_end(args); 
    
    Serial.print(str); 
    delete []str;
#else 
    char buf[120];
    vsprintf(buf,s,args);
    va_end(args);

    Serial.print(buf);
#endif

}
