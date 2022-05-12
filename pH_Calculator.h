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

#ifndef PH_CALCULATOR_H_
#define PH_CALCULATOR_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

//calibrates turret
void calibrate();
//measure tube's raw RGB values
void measure(uint8_t tubeNum);
//measures tube's pH
void measurepH(uint8_t tubeNum);




#endif /* PH_CALCULATOR_H_ */
