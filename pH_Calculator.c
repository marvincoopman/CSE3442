// Marvin Coopman
// 1001781933
// pH_Calculator

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
#include "pH_Calculator.h"

//-----------------------------------------------------------------------------
// Global variables
//-----------------------------------------------------------------------------

#define numOfKnownpH 5
uint16_t pwm_r, pwm_b, pwm_g, analog_r, analog_b, analog_g;


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//calibrates turret
void calibrate()
{
    putsUart0("Starting Calibration\n");
    waitMicrosecond(10000);
    home();
    waitMicrosecond(10000);

    setRgbColor(0, 0, 0);
    uint16_t raw;
    uint16_t dc = 0;
    //turn on red
    raw = 0;
    dc = 0;
    waitMicrosecond(10000);
    while(raw < 3072 && dc < 1024)
    {
        setRgbColor(dc, 0, 0);
        waitMicrosecond(3000);
        raw = readAdc0Ss3();
        dc++;
    }
    analog_r = raw;
    pwm_r = dc;
    waitMicrosecond(5000);

    //turn on green
    raw = 0;
    dc = 0;
    while(raw < 3072 && dc < 1024)
    {
        setRgbColor(0, dc, 0);
        waitMicrosecond(3000);
        raw = readAdc0Ss3();

        dc++;
    }
    analog_g = raw;
    pwm_g = dc;
    waitMicrosecond(5000);

    //turn on blue
    raw = 0;
    dc = 0;
    while(raw < 3072 && dc < 1024)
    {
        setRgbColor(0, 0, dc);
        waitMicrosecond(3000);
        raw = readAdc0Ss3();
        dc++;
    }
    analog_b = raw;
    pwm_b = dc;
    waitMicrosecond(5000);
    setRgbColor(0, 0, 0);

    if(pwm_r == 1024 || pwm_g == 1024 || pwm_b == 1024 )
    {
        putsUart0("Calibration Failed (Rerunning \"Calibration\" Recommended)\n");
    }
    else
    {
        putsUart0("Calibration Complete\n");
    }
    char rawData[100];
    sprintf(rawData, "PWM Calibration Values: (%d, %d, %d)\n", pwm_r, pwm_g, pwm_b);
    putsUart0(rawData);
    sprintf(rawData, "Analog Calibration Values: (%d, %d, %d)\n\n", analog_r, analog_g, analog_b);
    putsUart0(rawData);
}
//measure tube's raw RGB values
void measure(uint8_t tubeNum)
{
    if(tubeNum > 5)
    {
        return;
    }
    turnto(tubeNum);
    char rawData[100];
    sprintf(rawData,"Measuring %d...\n", tubeNum);
    putsUart0(rawData);

    turnto(tubeNum);
    waitMicrosecond(200000);

    setRgbColor(pwm_r, 0, 0);
    waitMicrosecond(100000);
    uint16_t r = readAdc0Ss3();

    setRgbColor(0, pwm_g, 0);
    waitMicrosecond(100000);
    uint16_t g = readAdc0Ss3();

    setRgbColor(0, 0, pwm_b);
    waitMicrosecond(100000);
    uint16_t b = readAdc0Ss3();
    setRgbColor(0, 0, 0);


    putsUart0("Measurement finished\n");
    sprintf(rawData, "Analog Measurement Values: (%d, %d, %d)\n\n", r, g, b);
    putsUart0(rawData);
}


//returns pH level
void measurepH(uint8_t tubeNum)
{
    char rawData [100];
    if(tubeNum == 0)
    {
        putsUart0("Cannot measure the pH level of the Reference Tube\n\n");
        return;
    }
    if(tubeNum > 5)
    {
        return;
    }
    sprintf(rawData,"Measuring %d...\n", tubeNum);
    putsUart0(rawData);
    double estimatedpH = -1;
    turnto(tubeNum);
    waitMicrosecond(200000);
    setRgbColor(pwm_r, 0, 0);
    waitMicrosecond(100000);
    uint16_t r = readAdc0Ss3();

    //print to terminal
    setRgbColor(0, pwm_g, 0);
    waitMicrosecond(100000);
    uint16_t g = readAdc0Ss3();

    //print to terminal
    setRgbColor(0, 0, pwm_b);
    waitMicrosecond(100000);
    uint16_t b = readAdc0Ss3();
    setRgbColor(0, 0, 0);


    double tempDistance = 0;
    double tempDistanceG = 0;
    double tempDistanceB = 0;
    double tempDistanceR = 0;





    tempDistanceR = (double)r / analog_r; //READVALUE/ CALIBRATED VALUE ex : 1577/3070
    tempDistanceG = (double)g / analog_g;
    tempDistanceB = (double)b / analog_b;


    tempDistance = tempDistanceG * tempDistanceG + (tempDistanceG*tempDistanceB) * (tempDistanceG*tempDistanceB); // = G^2 + (G*B)^2
    tempDistance /= (tempDistanceR + tempDistanceG + tempDistanceB); //DIVIDE BY SUM OF RED BLUE GREEN
    double x = 10 * log10(tempDistance); //LOG IT


    estimatedpH = (x - 55.71133) / (-8.465);
    sprintf(rawData,"tempDistance: %f \nLog value: %f \n",  tempDistance, x);
    putsUart0(rawData);
    putsUart0("Measurement finished\n");
    sprintf(rawData, "Estimated pH using general formula: %lf\n\n", estimatedpH);
    putsUart0(rawData);

}
