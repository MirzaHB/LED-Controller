
#include "TimeDelay.h"
#include "xc.h"

void __attribute__((interrupt, no_auto_psv)) _T2Interrupt(void)
{
    int num;
    IFS0bits.T2IF = 0; // Clear the Timer2 interrupt flag in the Interrupt Flag Status Register 0
    T2CONbits.TON = 0; // Turn off (disable) Timer2 to stop the timer
    num = 1;           // Assign the value 1 to the variable 'num'
}

void delay_ms(uint16_t time_ms)
{
    NewClk(32);         // Change the clock frequency to 32kHz for more precise timing
    TMR2 = 0;           // Reset the Timer2 counter to zero
    PR2 = time_ms << 4; // Set the period register (PR2) for Timer2 based on the desired time in milliseconds
                        // Shifting left by 4 bits is equivalent to multiplying by 16 (each count represents 1/16th of a millisecond)
    T2CONbits.TON = 1;  // Turn on Timer2 to start the timing operation
    Idle();             // Enter idle mode, pausing program execution until Timer2 completes its countdown

    return;
}
