
// Marvin Coopman
// 1001781933
// CSE 3442 Semester Project

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "math.h"
#include "clock.h"
#include "wait.h"
#include "uart0.h"
#include "adc0.h"
#include "tm4c123gh6pm.h"
#include "rgb_led.h"
#include "serial.h"
#include "motor.h"
#include "IR_Receiver.h"
#include "pH_Calculator.h"


// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();
}


int main(void)
{

    initHw();
    initRgb();
    setRgbColor(0, 0, 0);
    initAdc0Ss3();
    setAdc0Ss3Mux(11);
    setAdc0Ss3Log2AverageCount(2);
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initMotor();
    initIR();
    USER_DATA data;
    button button1;
    char* buttonName;


    while(true)
    {

        if(isValid())
        {
            buttonName = findButton(&button1);

            if(stringEqual(buttonName, "PW BUTTON"))
            {
                calibrate();
            }
            else if(stringEqual(buttonName, "WHITE0 BUTTON"))
            {
                home();
            }
            else if(stringEqual(buttonName, "DAYDOWN BUTTON"))
            {
                measure(0);
            }
            else if(stringEqual(buttonName, "RED0 BUTTON"))
            {
                measure(1);
            }
            else if(stringEqual(buttonName, "RED1 BUTTON"))
            {
                measure(2);
            }
            else if(stringEqual(buttonName, "RED2 BUTTON"))
            {
                measure(3);
            }
            else if(stringEqual(buttonName, "RED3 BUTTON"))
            {
                measure(4);
            }
            else if(stringEqual(buttonName, "RED4 BUTTON"))
            {
                measure(5);
            }
            else if(stringEqual(buttonName, "DAYUP BUTTON"))
            {
                measurepH(0);
            }
            else if(stringEqual(buttonName, "GREEN0 BUTTON"))
            {
                measurepH(1);
            }
            else if(stringEqual(buttonName, "GREEN1 BUTTON"))
            {
                measurepH(2);
            }
            else if(stringEqual(buttonName, "GREEN2 BUTTON"))
            {
                measurepH(3);
            }
            else if(stringEqual(buttonName, "GREEN3 BUTTON"))
            {
                measurepH(4);
            }
            else if(stringEqual(buttonName, "GREEN4 BUTTON"))
            {
                measurepH(5);
            }
            else if(stringEqual(buttonName, "PLAY BUTTON"))
            {
                turnto(0);
            }
            else if(stringEqual(buttonName, "BLUE0 BUTTON"))
            {
                turnto(1);
            }
            else if(stringEqual(buttonName, "BLUE1 BUTTON"))
            {
                turnto(2);
            }
            else if(stringEqual(buttonName, "BLUE2 BUTTON"))
            {
                turnto(3);
            }
            else if(stringEqual(buttonName, "BLUE3 BUTTON"))
            {
                turnto(4);
            }
            else if(stringEqual(buttonName, "BLUE4 BUTTON"))
            {
                turnto(5);
            }

            clearCode();
        }
        if(kbhitUart0())
        {
            getsUart0(&data);
            parseFields(&data);
            if(isCommand(&data, "calibrate" , 0))
            {
               calibrate();
            }
            else if(isCommand(&data, "home" , 0))
            {
                home();
            }
            else if(isCommand(&data, "turnto" , 1))
            {
                if(stringEqual(getFieldString(&data,2), "R"))
                {
                    turnto(0);
                }
                else
                {
                    turnto(getFieldInteger(&data, 1));
                }
            }
            else if(isCommand(&data,"measure",2))
            {
               if(stringEqual(getFieldString(&data,2), "pH"))
               {
                   if(stringEqual(getFieldString(&data,1), "R"))
                   {
                      measurepH(0);
                   }
                   else
                   {
                      measurepH(getFieldInteger(&data, 1));
                   }
               }
               else if(stringEqual(getFieldString(&data, 2), "raw"))
               {
                   if(stringEqual(getFieldString(&data, 1), "R"))
                   {
                      measure(0);
                   }
                   else
                   {
                      measure(getFieldInteger(&data, 1));
                   }
               }
           }
        }

    }

}
