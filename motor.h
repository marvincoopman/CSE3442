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

#ifndef MOTOR_H_
#define MOTOR_H_

//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

void stepCW();
void stepCCW();
void home();
void setPosition(uint8_t pos);
void turnto(uint8_t tubeNum);

void initMotor();
void applyPhase();



#endif /* MOTOR_H_ */
