// Marvin Coopman
// 1001781933
// IR_Receiver

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// PD2/WTIMER3A reads LED remote signal

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "wait.h"
#include "uart0.h"
#include "IR_Receiver.h"

#define T  22500     //time constant T



//Port D Mask
#define FREQ_IN_MASK 4 //D2

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

uint32_t time[50];
uint8_t code;
bool volatile  valid;
uint8_t count = 0;
uint8_t address;

button buttons[44] = {{0x00, 0x40, "PW BUTTON"},{0x00, 0x41, "PLAY BUTTON"},{0x00, 0x5D, "DAYDOWN BUTTON"},{0x00, 0x5C, "DAYUP BUTTON"},{0x00, 0x58, "RED0 BUTTON"},{0x00, 0x54, "RED1 BUTTON"},
{0x00, 0x50, "RED2 BUTTON"},{0x00, 0x1C, "RED3 BUTTON"},{0x00, 0x18, "RED4 BUTTON"},{0x00, 0x59, "GREEN0 BUTTON"},{0x00, 0x55, "GREEN1 BUTTON"},{0x00, 0x51, "GREEN2 BUTTON"},
{0x00, 0x1D, "GREEN3 BUTTON"},{0x00, 0x19, "GREEN4 BUTTON"},{0x00, 0x45, "BLUE0 BUTTON"},{0x00, 0x49, "BLUE1 BUTTON"},{0x00, 0x4D, "BLUE2 BUTTON"},{0x00, 0x1E, "BLUE3 BUTTON"},
{0x00, 0x1A, "BLUE4 BUTTON"},{0x00, 0x44, "WHITE0 BUTTON"},{0x00, 0x48, "WHITE1 BUTTON"},{0x00, 0x4C, "WHITE2 BUTTON"},{0x00, 0x1F, "WHITE3 BUTTON"},{0x00, 0x1B, "WHITE4 BUTTON"},
{0x00, 0x14, "REDUP BUTTON"},{0x00, 0x10, "REDDOWN BUTTON"},{0x00, 0x15, "GREENUP BUTTON"},{0x00, 0x11, "GREENDOWN BUTTON"},{0x00, 0x16, "BLUEUP BUTTON"},{0x00, 0x12, "BLUEDOWN BUTTON"},
{0x00, 0x17, "QUICK BUTTON"},{0x00, 0x13, "SLOW BUTTON"},{0x00, 0x0C, "DIY1 BUTTON"},{0x00, 0x0D, "DIY2 BUTTON"},{0x00, 0x0E, "DIY3 BUTTON"},{0x00, 0x08, "DIY4 BUTTON"},
{0x00, 0x09, "DIY5 BUTTON"},{0x00, 0x0A, "DIY6 BUTTON"},{0x00, 0x0F, "AUTO BUTTON"},{0x00, 0x0B, "FLASH BUTTON"},{0x00, 0x04, "JUMP3 BUTTON"},{0x00, 0x05, "JUMP7 BUTTON"},
{0x00, 0x06, "FADE3 BUTTON"},{0x00, 0x07, "FADE7 BUTTON"}};

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

char* findButton(button* button1)
{
    uint8_t i;
    for(i = 0; i < 44; i++)
    {
        if(address == buttons[i].add && code == buttons[i].data)
        {
//            putsUart0(buttons[i].name);
//            putsUart0("\n");
            return buttons[i].name;
        }
    }
    return NULL;
}

//Configure WTimer3
void enableWTimer3()
{
    WTIMER3_CTL_R &= ~TIMER_CTL_TAEN;                // turn-off counter before reconfiguring
    WTIMER3_CFG_R = 4;                               // configure as 32-bit counter (A only)
    WTIMER3_TAMR_R = TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP | TIMER_TAMR_TACDIR; // configure for edge time mode, count up
    WTIMER3_CTL_R = TIMER_CTL_TAEVENT_NEG;           // measure time from neg edge to positive edge
    WTIMER3_IMR_R = TIMER_IMR_CAEIM;                 // turn-on interrupts
    WTIMER3_TAV_R = 0;                               // zero counter for first period
    WTIMER3_CTL_R |= TIMER_CTL_TAEN;                 // turn-on counter
    NVIC_EN3_R |= 1 << (INT_WTIMER3A-16-96);         // turn-on interrupt 116 (WTIMER3A)
}


// Initialize IR
void initIR()
{
    initSystemClockTo40Mhz();

    //Enable Clocks
    SYSCTL_RCGCWTIMER_R |= SYSCTL_RCGCWTIMER_R3;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3;
    _delay_cycles(3);


    //Configure PD2 for WTimer3
    GPIO_PORTD_AFSEL_R |= FREQ_IN_MASK;
    GPIO_PORTD_PCTL_R &= ~GPIO_PCTL_PD2_M;           //Enable alternative function for PD2
    GPIO_PORTD_PCTL_R |= GPIO_PCTL_PD2_WT3CCP0;     //configure wide timer 3A
    GPIO_PORTD_DEN_R |= FREQ_IN_MASK;               //Enables PD2

    enableWTimer3();
}

//generates code from signal received
void createCode()
{
    uint8_t temp = 0;
    uint8_t tempAddress = 0;
    uint8_t tempData = 0;           //carry recorded address and data
    uint32_t timeTemp1, timeTemp2;  //carries temporary time interval to find '0's and '1's

    uint8_t i;


    //67500 = 3T <3T means '0' >3T means '1'
    for(i = 1; i < 8; i++)     //address portion
    {
        timeTemp1 = time[i+1] - time[i];
        timeTemp2 = time[i+8+1] - time[i+8];
        if(timeTemp1 < (67500))
        {
            temp = 0;       //address @ i is 0
            if(timeTemp2 < (67500))       //if !value was 0 code invalid and return
            {
                valid = false;
                return;
            }
        }
        else
        {
            temp = 1;       //address @ i is 1
            if(timeTemp2 > (67500))   //if !value was 1 code invalid and return
            {
                valid = false;
                return;
            }
        }
        temp = temp << i-1; //shifts 1/0 to proper bit location "i-1" fixes offset from for loop
        tempAddress += temp;
    }
    for(i = 17; i < 24; i++) //data portion
    {
        timeTemp1 = time[i+1] - time[i];
        timeTemp2 = time[i+8+1] - time[i+8];
        if(timeTemp1 < (67500))   //already validated that time interval was a '1' or '0' so
        {                                   // so just checking if below 3T for '0' or above 3T for '1'
            temp = 0;       //address @ i is 0
            if(timeTemp2 < (67500))       //if !value was 0 code invalid and return
            {
                valid = false;
                return;
            }
        }
        else
        {
            temp = 1;       //address @ i is 1
            if(timeTemp2 > (67500))   //if !value was 1 code invalid and return
            {
                valid = false;
                return;
            }
        }
        temp = temp << (i-17); //shifts 1/0 to proper bit location "i-17" fixes offset from for loop
        tempData += temp;
    }
    code = tempData;
    address = tempAddress;
    valid = true;
}

//WTimer3 Interrupt Routine
//Validates signal is coming from LED remote
void wideTimer3Isr()
{
    if(WTIMER3_TAV_R > 4000000)
    {
        count = 0;
    }

    if(count == 0)  //if first edge
    {
        WTIMER3_TAV_R = 0;                           // zero counter for next edge
        time[count] = WTIMER3_TAV_R;
        count++;
    }
    else if(count == 1)
    {
        time[count] = WTIMER3_TAV_R;
        //if time between edges is 13 - 14ms 23*T = 520000 clocks 24*T = 560000 clocks
        if((time[1] - time[0]) >= (520000) && (time[1] - time[0]) <= (560000) )
        {
            count++;                    //increment count
        }
        else
        {
            count = 0;                  //reset if invalid start to IR command
        }
    }
    else
    {
        time[count] = WTIMER3_TAV_R;
        uint32_t timeInterval = time[count] - time[count-1];
        //if interval is a 2T or 4T within .5T
        //1.5T = 33750      2.5T = 56250      3.5T = 78750      4.5T = 101250
        if((timeInterval >= (33750) && timeInterval <= (56250)) || (timeInterval >= (78750) && timeInterval <= (101250)))
        {
            count++;        //increment
        }
        else
        {
            count = 0;      //invalid bit length reset
        }
    }
    if(count >= 34) //Finished receiving signal
    {
        createCode();
        count = 0;
    }
    WTIMER3_ICR_R = TIMER_ICR_CAECINT;           // clear interrupt flag
}


bool isValid()
{
    return valid;
}


void clearCode()
{
    code = 0;
    address = 0;
    valid = false;
}

