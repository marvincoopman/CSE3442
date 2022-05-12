// RGB LED Library
// Jason Losh

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL with LCD Interface
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red Backlight LED:
//   M1PWM5 (PF1) drives an NPN transistor that powers the red LED
// Green Backlight LED:
//   M1PWM7 (PF3) drives an NPN transistor that powers the green LED
// Blue Backlight LED:
//   M1PWM6 (PF2) drives an NPN transistor that powers the blue LED

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <rgb_led.h>
#include <stdint.h>
#include "tm4c123gh6pm.h"

// PortF masks
#define RED_LED_MASK 2 //PF1
#define BLUE_LED_MASK 1 //PD0
#define GREEN_LED_MASK 2 //PD1



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize RGB
void initRgb()
{
    //PF1 PD0 PD1
    //M1PWM5 M1PWM0 M1PWM1
    // Enable clocks
    SYSCTL_RCGCPWM_R |= SYSCTL_RCGCPWM_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    // Configure three LEDs
    GPIO_PORTF_DEN_R |= RED_LED_MASK;
    GPIO_PORTF_AFSEL_R |= RED_LED_MASK;
    GPIO_PORTF_PCTL_R &= ~(GPIO_PCTL_PF1_M);
    GPIO_PORTF_PCTL_R |= GPIO_PCTL_PF1_M1PWM5;

    GPIO_PORTD_DEN_R |= GREEN_LED_MASK | BLUE_LED_MASK;
    GPIO_PORTD_AFSEL_R |= GREEN_LED_MASK | BLUE_LED_MASK;
    GPIO_PORTD_PCTL_R &= ~(GPIO_PCTL_PD0_M | GPIO_PCTL_PD1_M);
    GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD0_M1PWM0 | GPIO_PCTL_PD1_M1PWM1;

    // Configure PWM module 1 to drive RGB LED
    // RED   on M1PWM0 (PD0), M1PWM0a
    // BLUE  on M1PWM1 (PD1), M1PWM0b
    // GREEN on M1PWM5 (PF1), M1PWM2b
    SYSCTL_SRPWM_R = SYSCTL_SRPWM_R1;                // reset PWM1 module
    SYSCTL_SRPWM_R = 0;                              // leave reset state
    PWM1_0_CTL_R = 0;                                // turn-off PWM1 generator 0 (drives outs 0 and 1)
    PWM1_2_CTL_R = 0;                                // turn-off PWM1 generator 2 (drives outs 4 and 5)
    PWM1_0_GENA_R = PWM_1_GENA_ACTCMPAD_ONE | PWM_1_GENA_ACTLOAD_ZERO;
                                                        // output 0 on PWM1, gen 0a, cmpa

    PWM1_0_GENB_R = PWM_1_GENB_ACTCMPBD_ONE | PWM_1_GENB_ACTLOAD_ZERO;
                                                     // output1 on PWM1, gen 0b, cmpb

    PWM1_2_GENB_R = PWM_1_GENB_ACTCMPBD_ONE | PWM_1_GENB_ACTLOAD_ZERO;
                                                     // output 5 on PWM1, gen 2b, cmpb

    PWM1_0_LOAD_R = 2048;                            // set frequency to 40 MHz sys clock / 2 / 1024 = 19.53125 kHz
    PWM1_2_LOAD_R = 4096;

    PWM1_0_CMPA_R = 0;                               // red off (0=always low, 1023=always high)
    PWM1_0_CMPB_R = 0;                               // green off
    PWM1_2_CMPB_R = 0;                               // blue off

    PWM1_0_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM1 generator 2
    PWM1_2_CTL_R = PWM_0_CTL_ENABLE;                 // turn-on PWM1 generator 3
    PWM1_ENABLE_R = PWM_ENABLE_PWM0EN | PWM_ENABLE_PWM1EN | PWM_ENABLE_PWM5EN;
                                                     // enable outputs
}

void setRgbColor(uint16_t red, uint16_t green, uint16_t blue)
{
    PWM1_0_CMPA_R = red;
    PWM1_0_CMPB_R = blue;
    PWM1_2_CMPB_R = green;
}


