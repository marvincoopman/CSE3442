// Marvin Coopman
// 1001781933
// Motor

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Pins A5, A6, A7, and B4 configured as GPO outputs to run the motor


//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------


#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "clock.h"
#include "tm4c123gh6pm.h"
#include "wait.h"
#include "motor.h"


// Bitband Aliases
#define motorBlack (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 5*4))) //pin A5
#define motorGreen (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 6*4))) //pin A6
#define motorYellow (*((volatile uint32_t *)(0x42000000 + (0x400043FC-0x40000000)*32 + 7*4))) //pin A7
#define motorWhite (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 4*4))) //pin B4

//Port A Mask
#define MOTORGMASK 64
#define MOTORYMASK 128
#define MOTORBMASK 32

//Port B Mask
#define MOTORWMASK 16



//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

enum _phase{phase0 = 0,phase1,phase2,phase3};
uint8_t tubePos[6] = {3, 170, 137, 103, 69, 36};
uint8_t position = 0;
uint8_t temp = 0;
enum _phase currentPhase = phase0;



//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void stepCCW()
{
    if(position == 199)
   {
       position = 0;
   }
    else
    {
        position++;
    }

   if(currentPhase == phase3)
   {
       currentPhase = phase0;
   }
   else
   {
       currentPhase++;
   }

   waitMicrosecond(20000);
   applyPhase();

}
void stepCW()
{
    if(position == 0)
    {
        position = 200;
    }
    if(currentPhase == phase0)
    {
        currentPhase = phase3;
    }
    else
    {
        currentPhase--;
    }

    position--;
    waitMicrosecond(20000);
    applyPhase();
}
//Resets position at tube 0 (Reference tube)
void home()
{
    uint8_t i;
    for(i = 0; i< 200; i++)
    {
        stepCW();
    }
    waitMicrosecond(20000);
    position = 0;
    turnto(0);
}

void setPosition(uint8_t pos)
{
    position = pos;
}

//Turns motor to desired tube position
void turnto(uint8_t tubeNum)
{
    //positions are stored in global array tubePos[]
    if(tubeNum > 6) //out of range
    {
        return;
    }
    while(position != tubePos[tubeNum])
    {
        if(position > tubePos[tubeNum]) //CCW
        {
            stepCW();
        }
        else if(position < tubePos[tubeNum]) //CW
        {
            stepCCW();
        }
        if(position == tubePos[tubeNum])
        {
            return;
        }
    }
    //otherwise position is already at tubePos so do nothing
}
// Initialize Motor
void initMotor()
{
    initSystemClockTo40Mhz();

    //Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R0 | SYSCTL_RCGCGPIO_R1;
    _delay_cycles(3);

    //Configure GPO pins as outputs
    GPIO_PORTA_DIR_R |= MOTORGMASK | MOTORBMASK | MOTORYMASK;  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTA_DR2R_R |= MOTORGMASK | MOTORBMASK | MOTORYMASK;
    GPIO_PORTA_DEN_R |= MOTORGMASK | MOTORBMASK | MOTORYMASK;
    GPIO_PORTB_DIR_R |= MOTORWMASK;  // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTB_DR2R_R |= MOTORWMASK;
    GPIO_PORTB_DEN_R |= MOTORWMASK;

    home();
}

//Changes phase to rotate the motor
void applyPhase()
{
    switch(currentPhase)
    {
        case 0:
            motorWhite = 0; //B4
            motorBlack = 0; //A5
            motorYellow = 0; //A7
            motorGreen = 1; //A6
            break;
        case 1:
            motorWhite = 0;
            motorBlack = 1;
            motorYellow = 0;
            motorGreen = 0;
            break;
        case 2:
            motorWhite = 0;
            motorBlack = 0;
            motorYellow = 1;
            motorGreen = 0;
            break;
        case 3:
            motorWhite = 1;
            motorBlack = 0;
            motorYellow = 0;
            motorGreen = 0;
            break;
        default:
            break;
    }
}

