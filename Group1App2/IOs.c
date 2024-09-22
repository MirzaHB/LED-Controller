

#include "xc.h"

#include "ChangeClk.h"
#include "TimeDelay.h"
#include "IOs.h"
#include "UART2.h"
#include "ADC.h"

int PB1Pressed = 0;
int PB2Pressed = 0;
void __attribute__((interrupt, no_auto_psv)) _CNInterrupt(void)
{
    IFS1bits.CNIF = 0; // Clear CN interrupt flag
    NewClk(32);        // Change clock to 32kHz for debouncing
    delay_ms(100);     // De-bounce period
    NewClk(8);         // set clock back to 8MHz

    LATBbits.LATB8 = 0; // Turn off the led

    // Check if PB1 is pressed
    if (!PORTAbits.RA2)
    {
        switch (PB1Pressed)
        {
        case 0:
            PB1Pressed = 1; // Set the flag indicating PB1 is pressed
            IOcheck(1);     // Set the bulb intensity based on the potentiometer
            break;
        case 1:
            PB1Pressed = 0; // Reset the flag indicating PB1 is not pressed
            delay_ms(100);  // Delay for debouncing
            IOcheck(0);     // Set the system in sleep mode
            break;
        }
    }

    // Check if PB2 is pressed
    if (!PORTAbits.RA4)
    {
        switch (PB2Pressed)
        {
        case 0:
            PB2Pressed = 1; // Set the flag indicating PB2 is pressed
            IOcheck(2);     // Set the bulb to flicker every 0.5 seconds
            break;
        case 1:
            PB2Pressed = 0; // Reset the flag indicating PB2 is not pressed
            delay_ms(100);  // Delay for debouncing
            IOcheck(0);     // Set the system in sleep mode
            break;
        }
    }
}

//// IOinit() with IO interrupts
void IOinit(void)
{
    TRISBbits.TRISB8 = 0; // Make GPIO RB8 as a digital output
    LATBbits.LATB8 = 0;   // Make GPIO RB8 as a digital output

    TRISAbits.TRISA4 = 1; // Makes GPIO RA4 as a digital input
    CNPU1bits.CN0PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN0IE = 1;  // enable CN on CN0

    TRISBbits.TRISB4 = 1; // Makes GPIO RB4 as a digital input
    CNPU1bits.CN1PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN1bits.CN1IE = 1;  // enable CN on CN1

    TRISAbits.TRISA2 = 1;  // Makes GPIO RA2 as a digital input
    CNPU2bits.CN30PUE = 1; // Enables pull up resistor on RA4/CN0
    CNEN2bits.CN30IE = 1;  // enable CN on CN30

    // CN Interrupt settings
    IPC4bits.CNIP = 6; // 7 is highest priority, 1 is lowest, 0 is disabled interrupt
    IFS1bits.CNIF = 0; // Clear interrupt flag
    IEC1bits.CNIE = 1; // Enable CN interrupts

    return;
}

void Brightness(int level)
{
    if (level > 100 || level < 0)
    {
        return; // if value is not a percentage return
    }

    int period = (int)((level / 100.0) * 2000);

    // Turn on the light bulb for the specified period
    if (period != 0)
    {
        LATBbits.LATB8 = 1;
        PR2 = period * 4;  // 4 cycles per microsecond
        T2CONbits.TON = 1; // Start the timer
        Idle();
        T2CONbits.TON = 0; // Stop the timer
    }

    // Delay for the remaining period with the bulb off
    int remainingPeriod = 2000 - period;
    if (remainingPeriod != 0)
    {
        LATBbits.LATB8 = 0;
        PR2 = remainingPeriod * 4; // 4 cycles per microsecond
        T2CONbits.TON = 1;         // Start the timer
        Idle();
        T2CONbits.TON = 0; // Stop the timer
    }

    LATBbits.LATB8 = 0;
}

void IOcheck(int currentState)
{
    if (currentState == 1)
    {
        // Continuous loop for reading ADC, adjusting brightness, and displaying values
        Brightness(100); // Set initial brightness to 100%
        while (1)
        {
            uint16_t value = do_ADC();                      // Read ADC value
            float brightness = ((float)value / 1023) * 100; // Convert ADC value to brightness percentage
            Brightness(brightness);                         // Adjust brightness based on ADC value
            Disp2Dec(brightness);                           // Display the intensity % and send to Python
            XmitUART2('\n', 1);                             // Transmit a newline character
        }
    }
    else if (currentState == 2)
    {
        // Keep 100% brightness while blinking at 0.5-second intervals
        while (1)
        {
            Brightness(100); // Set brightness to 100%
            delay_ms(500);   // Delay for 0.5 seconds
            Brightness(0);   // Set brightness to 0%
            delay_ms(500);   // Delay for another 0.5 seconds
        }
    }
    else
    {
        LATBbits.LATB8 = 0;
        Idle(); // Enter low power mode
    }
    return;
}
