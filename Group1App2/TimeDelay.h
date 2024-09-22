
// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef TimeDelay_H
#define TimeDelay_H

#include <xc.h> // include processor files - each processor file is guarded.

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    // TODO If C++ is being used, regular C code needs function names to have C
    // linkage so the functions can be used by the c code.

#ifdef __cplusplus
}
#endif /* __cplusplus */

extern unsigned int TMR2flag;

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void);

void delay_ms(unsigned int time_ms);

#endif
