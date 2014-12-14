#ifndef _RTT

#ifdef __cplusplus
extern "C" {
#endif

void delay(int);
void init();
unsigned int millis();

#ifdef __cplusplus
}
#endif

#define _RTT
#endif